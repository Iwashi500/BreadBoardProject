from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF, renderPM
import sys
import os


def changeSVG2PNG(filename):
    filename_without_ext = os.path.splitext(os.path.basename(filename))[0]
    drawing = svg2rlg(filename)
    renderPM.drawToFile(drawing, './Python/' +
                        filename_without_ext + ".png", fmt="PNG")
