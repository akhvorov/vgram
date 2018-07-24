

from sklearn.datasets import fetch_20newsgroups
# from vgram import VGram
import time

def to_vect(text, map):
    words = text.strip().split(' ')
    vec = [-1 for i in range(len(words))]
    for i in range(len(words)):
        if words[i] not in map.keys():
            map[words[i]] = len(map)
        vec[i] = map[words[i]]
    return vec

def main():
    X_tr = fetch_20newsgroups(subset='train')
    # X_te = fetch_20newsgroups(subset='test')
    t = time.time()
    map = {}
    vecs = []
    for text in X_tr.data:
        vecs.append(to_vect(text, map))
    print('time to execution:', time.time() - t, 'sec')


if __name__ == "__main__":
    main()