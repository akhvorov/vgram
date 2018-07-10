//
// Created by akhvorov on 08.07.18.
//

#include <iostream>
#include <exception>


DictExpansion::DictExpansion(std::vector<char> const & alphabet, final int size) {
    this(alphabet, size, nullptr);
}

// TODO
DictExpansion::DictExpansion(std::vector<char> const & alphabet, std::size_t size, boolean trace) {
    this(new ListDictionary<>(ArrayTools.toArray(alphabet)), size, trace ? System.out : null);
}

// TODO
DictExpansion::DictExpansion(std::vector<char> const & alphabet, std::size_t size, PrintStream trace) {
    this(new ListDictionary<>(ArrayTools.toArray(alphabet)), size, trace);
}

DictExpansion::DictExpansion(IDictionary const & alphabet, std::size_t size) {
    this(alphabet, size, nullptr);
}

DictExpansion::DictExpansion(IDictionary const & alphabet, std::size_t size, PrintStream trace) {
    this.size = size;
    this.trace = trace;
    this.alphabetSize = alphabet.size();
    initial = alphabet;
    //isDynamic = !(alphabet instanceof ListDictionary); //TODO !!!
    current = createDict((List<Seq<T>>)alphabet.alphabet(), null, isDynamic, MAX_MIN_PROBABILITY);
}

DictExpansion::DictExpansion(std::int32_t slots) {
    this(IDictionary.EMPTY, slots, null); //TODO make EMPTY
}

DictExpansion::DictExpansion(int size, PrintStream const & trace) {
    this(Dictionary.EMPTY, size, trace); //TODO make EMPTY
}

DictionaryWithStat DictExpansion::createDict(std::vector<std::string> const & alphabet, std::vector<std::int32_t> const & initFreqs,
                                             boolean isDynamic, double minProbResult) {
    if (initFreqs != nullptr && initFreqs.size() != alphabet.size())
        throw std::invalid_argument();
    // TODO impl it
    return DictionaryWithStat(isDynamic ? DynamicDictionary(alphabet) : ListDictionary(alphabet.toArray(new Seq[alphabet.size()])), initFreqs, minProbResult);
}

IDictionary DictExpansion::result() {
    return result != nullptr ? result.dict : nullptr;
}

IDictionary DictExpansion::alpha() {
    return initial;
}

void DictExpansion::accept(std::string const & seq) {
    boolean enough;
    current.parse(seq);
    enough = (current.enough(probFound) || current.power > MAX_POWER);
    if (enough)
        update();
}

void DictExpansion::update() {
    if (!((current.enough(probFound) || current.power > MAX_POWER)))
        return;
    double sum = 0;
    double textLength = 0;
    for (std::size_t i = 0; i < current.size(); ++i) {
        std::int32_t freq = current.freq(i);
        textLength += current.get(i).length() * freq;
        if (freq > 0)
            sum -= freq * log(freq) / log(2);
    }
    double codeLength = (sum + current.power * log(current.power) / log(2)) / 8.;
    double compressionRate = codeLength / textLength;
    if (compressionRate < bestCompressionRate) {
        bestCompressionRate = compressionRate;
        noRateIncreaseTurns = 0;
    } else if (++noRateIncreaseTurns > 3) {
        probFound *= 0.8;
    }

    DictionaryWithStat result;
    if (populate) {
        result = current;
        if (trace != null)
            trace << "Size: " + current.size() + " rate: " + compressionRate + " minimal probability: " + current.minProbability << std::endl;
        std::int32_t slots;
        if (current.size() * EXTENSION_FACTOR < 10)
            slots = size - alphabetSize;
        else
            slots = (std::int32_t)(current.size() * EXTENSION_FACTOR);
        result = current.expand(slots, isDynamic);
    }
    else {
        this.result = result = current.reduce(size - alphabetSize, isDynamic);
    }
    current = result;
    populate = !populate;
}

std::vector<std::int32_t> DictExpansion::resultFreqs() {
    if (result.size() > result.symbolFreqs.size())
        for (std::size_t i = result.symbolFreqs.size(); i < result.size(); ++i)
            result.symbolFreqs.push_back(0);
    return result.symbolFreqs;
}

void DictExpansion::printPairs(std::ofstream const & ps) {
        std::vector<std::string> alphabet = result.alphabet();
        std::vector<std::int32_t> indices = new TIntArrayList();
        std::vector<double> weights;
        std::vector<std::int32_t> indicesArr(result().size());
        std::vector<double> weightsArr(result().size());

        ps << "{\n";
        for (std::size_t i = 0; i < alphabet.size(); ++i) {
            std::string tSeq = alphabet.get(i);
            indices.clear();
            weights.clear();
            // TODO change
            result.visitAssociations(i, (j, val) -> {
                indices.push_back(j);
                weights.push_back(val);
                return false;
            });
            weights.toArray(weightsArr, 0, weights.size());
            indices.toArray(indicesArr, 0, indices.size());
            ArrayTools.parallelSort(weightsArr, indicesArr, 0, indices.size());

            final String symbol = tSeq.toString();
            ps.append(JSONTools.escape(symbol)).append(": {");
            for (int j = indices.size() - 1; j >= 0 && weightsArr[j] > 0.001; j--) {
                if (j != indices.size() - 1)
                    ps.append(",");
                final String expansion = alphabet.get(indicesArr[j]).toString();
                ps.append("\n").append(JSONTools.escape(expansion)).append(": ").append(CharSeqTools.ppDouble(weightsArr[j] / (double) result.freq(i)));
            }
            ps.append("\n},\n");
        }
        ps << "}\n";
}

void DictExpansion::print(std::ofstream const & fileWriter) {
    for (std::size_t i = 0; i < result.size(); ++i) {
        std::string seq = result.get(i);
        fileWriter << seq;
        fileWriter << '\t';
        fileWriter << result.freq(i);
        //fileWriter.append('\n');
        fileWriter << "\n\n";
    }
}

double DictExpansion::codeLength() {
    return result.codeLengthPerChar();
}

// not impl !!!
double DictExpansion::kl(std::vector<std::int32_t> const & freqs, std::unordered_map<std::int64_t, std::int32_t> const & pairFreqs) {
    std::vector<double> freqXFirst(freqs.dim());
    for (auto &e : pairFreqs) {
        std::int32_t code = e.first;
        std::int32_t freq = e.second;
        freqXFirst[(std::int32_t)(code >> 32)] = freq;
    }

    double totalPairFreqs = std::accumulate(freqXFirst.begin(), freqXFirst.end(), 0);
    double totalFreqs = std::accumulate(freqs.begin(), freqs.end(), 0);

    double result[] = {0};
    for (auto &e : pairFreqs) {
        std::int32_t code = e.first;
        std::int32_t freq = e.second;
        std::int32_t first = (int) (code >>> 32);
        std::int32_t second = (int) (code & 0xFFFFFFFFL);
        double pAB = freq / totalPairFreqs;
        double pBcondA = freq / freqXFirst.get(first);
        double pA = freqs.get(first) / totalFreqs;
        double pB = freqs.get(second) / totalFreqs;
        result[0] += freq * pBcondA * log(pAB / pA / pB);
    }
    return result[0];
}
