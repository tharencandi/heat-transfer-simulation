import cv2 as cv
import numpy as np

def get_arr(file):
    CSVData = open(file)
    arr = np.genfromtxt(CSVData, delimiter=";")
    return arr

def assert_equal(test, true):
    print(f"test shape: {test.shape} | true shape: {true.shape}")
    assert np.array_equal(test, true)

def vis_mat(arr):
    cv.normalize(arr, arr, 255, 0)
    cv.imshow("mat", arr)
    cv.waitKey()


true = get_arr("original.csv")
test = get_arr("test.csv")

vis_mat(true)
vis_mat(test)

assert_equal(test, true)


