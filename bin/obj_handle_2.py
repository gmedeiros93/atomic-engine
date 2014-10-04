import sys, struct
from math import sqrt

def load_obj(path):
    verts = []
    norms = []

    verts_out = []
    norms_out = []

    for line in open(path, "r"):
        vals = line.split()

        if len(vals) < 1:
            continue

        if vals[0] == "v":
            verts.append(scale(tuple(map(float, vals[1:4])), 0.05))

        if vals[0] == "vn":
            norms.append(tuple(map(float, vals[1:4])))

        if vals[0] == "vt":
            #print("found texcoord")
            pass

        if vals[0] == "f":
            face = []

            for f in vals[1:]:
                w = f.split("/", 2)
                w += ["0"] * (3 - len(w))
                w = map(int, w)
                w = list(map(lambda v: len(verts) + v if v < 0 else v - 1, w))

                try:
                    face.append(list(verts[int(w[0]) - 1]))
                    #verts_out.append(list(verts[int(w[0]) - 1]))
                except Exception as e:
                    print(line)
                    print(int(w[0]) - 1)
                    break

            if len(face) == 3:
                verts_out += face
            if len(face) == 4:
                verts_out += [face[0], face[1], face[2]]
                #verts_out += [face[2], face[3], face[0]]
                verts_out += [face[0], face[2], face[3]]

    #return verts_out, norms_out
    return verts_out

import random
def to_vbo(verts, norms):
    vbo = []

    for vert, norm in zip(verts, norms):
        #print(vert+[0.0,0.0]+norm)
        vbo += vert
        vbo += [random.random(), random.random()]
        vbo += norm

    return vbo

def calc_norms(verts):
    norms = [[0.0, 0.0, 0.0]] * len(verts)

    for i in range(0, len(verts), 3):
        try:
            normal = normalize(cross(
                sub(verts[i + 1], verts[i]),
                sub(verts[i + 2], verts[i])
            ))

            norms[i] = normal
            norms[i + 1] = normal
            norms[i + 2] = normal
        except IndexError:
            continue

    return norms

def sub(a, b):
    return [x-y for x,y in zip(a,b)]

def length(vec):
    return sqrt(sum(map(lambda v: v*v, vec)))

def normalize(vec):
    l = length(vec)
    if l == 0:
        return vec
    return list(map(lambda v: v/l, vec))

def scale(vec, fac):
    return list(map(lambda v: v*fac, vec))

def cross(a, b):
    assert len(a)>=3
    assert len(b)>=3

    return [
        a[1]*b[2] - a[2]*b[1],
        a[2]*b[0] - a[0]*b[2],
        a[0]*b[1] - a[1]*b[0]
    ]

if __name__ == "__main__":
    #verts, norms = load_obj(sys.argv[1])
    verts = load_obj(sys.argv[1])
    norms = calc_norms(verts)
    vbo = to_vbo(verts, norms)
    floats = len(vbo)
    print("{} vertices, {} floats".format(len(verts), floats))
    print(vbo[5], vbo[6], vbo[7])
    vbo.insert(0, floats)
    data = struct.pack("I{}f".format(floats), *vbo)

    with open("out.bin", "wb") as fp:
        fp.write(data)