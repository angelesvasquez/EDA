import pandas as pd
import matplotlib.pyplot as plt

points = pd.read_csv("points.csv")
centroids = pd.read_csv("centroids.csv")

# puntos
plt.scatter(
    points["x"],
    points["y"],
    c=points["cluster"],
    cmap='tab20',
    alpha=0.7
)

# centroides
plt.scatter(
    centroids["x"],
    centroids["y"],
    c=range(len(centroids)),
    cmap='tab20',
    marker='X',
    s=300,
    edgecolors='black'
)

plt.show()