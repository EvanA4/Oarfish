import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


def show_discrete():
    data = pd.read_csv("discrete.txt").to_numpy()
    names = data[:, 0]
    times = data[:, 1:]
    nums = np.arange(64)
    plt.figure(figsize=(10, 6))
    for i in range(len(names)):
        plt.plot(nums, times[i], label=names[i])
    plt.legend()
    plt.title("Trailing-Zero Functions for Large Numbers")
    plt.xlabel("1 << Number")
    plt.ylabel("Time for 5,000,000 runs (ms)")
    plt.show()


def show_range():
    data = pd.read_csv("discrete.txt").to_numpy()
    names = data[:, 0]
    times = data[:, 1]
    plt.figure(figsize=(10, 6))
    for i in range(len(names)):
        plt.bar(names[i], times[i])
    plt.title("Scaling Performance for Counting")
    plt.xlabel("Name")
    plt.ylabel("Time to count to 500,000,000 (ms)")
    plt.show()


def main():
    show_discrete()
    show_range()


if __name__ == "__main__":
    main()