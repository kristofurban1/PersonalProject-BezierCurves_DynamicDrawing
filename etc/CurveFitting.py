import numpy as np
import matplotlib.pyplot as plt

def chord_length_parameterize(pts):
    d = np.linalg.norm(np.diff(pts, axis=0), axis=1)
    t = np.insert(np.cumsum(d), 0, 0)
    return t / t[-1]

def bezier_cubic(t, P0, P1, P2, P3):
    return (
        (1 - t)**3 * P0 +
        3 * (1 - t)**2 * t * P1 +
        3 * (1 - t) * t**2 * P2 +
        t**3 * P3
    )

def fit_cubic_bezier(pts):
    t = chord_length_parameterize(pts)
    P0 = pts[0]
    P3 = pts[-1]
    A = []
    bx = []
    by = []

    for i in range(1, len(pts) - 1):
        ti = t[i]
        a = 3 * (1 - ti)**2 * ti
        b = 3 * (1 - ti) * ti**2
        c = (1 - ti)**3 * P0 + ti**3 * P3
        A.append([a, b])
        bx.append(pts[i, 0] - c[0])
        by.append(pts[i, 1] - c[1])

    A = np.array(A)
    bx = np.array(bx)
    by = np.array(by)

    P1x, P2x = np.linalg.lstsq(A, bx, rcond=None)[0]
    P1y, P2y = np.linalg.lstsq(A, by, rcond=None)[0]

    P1 = np.array([P1x, P1y])
    P2 = np.array([P2x, P2y])

    return P0, P1, P2, P3

def evaluate_bezier(P0, P1, P2, P3, samples=100):
    t_vals = np.linspace(0, 1, samples)
    return np.array([bezier_cubic(t, P0, P1, P2, P3) for t in t_vals])

def fit_error(pts, curve_pts):
    from scipy.spatial import cKDTree
    tree = cKDTree(curve_pts)
    dists, _ = tree.query(pts)
    return np.mean(dists)