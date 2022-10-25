import cv2 as cv
import numpy as np


def vis_mat(file):
    CSVData = open(file)
    arr = np.genfromtxt(CSVData, delimiter=";")


    cv.normalize(arr, arr, 255, 0)
    cv.imshow("mat", arr)
    cv.waitKey()

vis_mat("output.csv")
vis_mat("output_2.csv")