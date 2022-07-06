
import schemdraw
import schemdraw.elements as elm
import math
import pandas as pd
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF, renderPM
import sys
import os


def changeSVG2PNG(filename):
    filename_without_ext = os.path.splitext(os.path.basename(filename))[0]
    drawing = svg2rlg(filename)
    renderPM.drawToFile(drawing,
                        filename_without_ext + ".png", fmt="PNG")


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return "x : {0}, y : {1}".format(self.x, self.y)

    def index(self):
        return self.y * 30 + self.x


def createPoint(x, y):
    if 2 <= y and y <= 11:
        return Point(x, y)
    elif x < 5:
        return Point(x, y)
    elif x < 10:
        return Point(x+1, y)
    elif x < 15:
        return Point(x+2, y)
    elif x < 20:
        return Point(x+3, y)
    elif x < 25:
        return Point(x+4, y)
    else:
        return Point(x, y)


def changePoint(point1, point2):
    if point1.y <= 1:
        if point2 <= 1:
            point2 = point2
        elif point2 <= 6:
            point2.y = 2
        elif point2 <= 11:
            point2.y = 7
    elif point1.y <= 6:
        if point2.y <= 1:
            point1.y = 2
        elif point2.y <= 6:
            point1 = point1
        elif point2.y <= 11:
            point1.y = 6
            point2.y = 7
        else:
            point1.y = 6
    elif point1.y <= 11:
        if point2.y <= 1:
            point1.y = 7
        elif point2.y <= 6:
            point1.y = 7
            point2.y = 6
        elif point2.y <= 11:
            point1 = point1
        else:
            point1.y = 11
    else:
        if point2.y <= 1:
            point1 = point1
        elif point2.y <= 6:
            point2.y = 6
        elif point2.y <= 11:
            point2.y = 11

    return point1, point2


def drawGND(d, point1, point2):
    if point1.y == 0:
        d += elm.Line().color(disable).endpoints(
            position[point1.index()-1].start, position[point1.index()].start)
        d += elm.GND().color(enable).left
    elif point1.y == 12:
        d += elm.Line().color(disable).endpoints(
            position[point1.index()-1].start, position[point1.index()].start)
        d += elm.GND().color(enable).right

    if point2.y == 0:
        d += elm.Line().color(disable).endpoints(
            position[point2.index()-1].start, position[point2.index()].start)
        d += elm.GND().color(enable).left
    elif point2.y == 12:
        d += elm.Line().color(disable).endpoints(
            position[point2.index()-1].start, position[point2.index()].start)
        d += elm.GND().color(enable).right


def drawVDD(d, point1, point2):
    if point1.y == 1:
        d += elm.Line().color(disable).endpoints(
            position[point1.index()-1].start, position[point1.index()].start)
        d += elm.VDD().color(enable).right
    elif point1.y == 13:
        d += elm.Line().color(disable).endpoints(
            position[point1.index()-1].start, position[point1.index()].start)
        d += elm.VDD().color(enable).left

    if point2.y == 1:
        d += elm.Line().color(disable).endpoints(
            position[point2.index()-1].start, position[point2.index()].start)
        d += elm.VDD().color(enable).right
    elif point2.y == 13:
        d += elm.Line().color(disable).endpoints(
            position[point2.index()-1].start, position[point2.index()].start)
        d += elm.VDD().color(enable).left


disable = 'white'
enable = 'black'

d = schemdraw.Drawing(unit=1.0, inches_per_unit=0.5)
d += (P := elm.Line().down().color(disable))
position = [P]*30*14

# 盤面制作
for i in range(14):
    for j in range(29):
        if(i % 2 == 0):
            position[30 * i + j] = d.add(elm.Line().right().color(disable))
        else:
            position[30 * i + (29 - j)
                     ] = d.add(elm.Line().left().color(disable))
    if(i % 2 == 0):
        position[30 * i + 29] = d.add(elm.Line().down().color(disable))
    else:
        position[30 * i + 0] = d.add(elm.Line().down().color(disable))

# csvファイル読み込み
csv = pd.read_csv(filepath_or_buffer=".\connects-parameter.csv")
for param in csv.values:
    t = param[0]
    point1 = createPoint(param[1], param[2])
    point2 = createPoint(param[3], param[4])

    # パーツ追加するごとに編集
    if param[0] == "wire":
        d += elm.Line().color(enable).endpoints(
            position[point1.index()].start,
            position[point2.index()].start
        )
    elif param[0] == "resistor":
        d += elm.Resistor().color(enable).endpoints(
            position[point1.index()].start,
            position[point2.index()].start
        )
    elif param[0] == "LED":
        d += elm.LED().color(enable).endpoints(
            position[point1.index()].start,
            position[point2.index()].start
        )
    elif param[0] == "condenser":
        d += elm.Capacitor(polar=True).color(enable).endpoints(
            position[point1.index()].start,
            position[point2.index()].start
        )
    elif param[0] == "switch":
        d += elm.Button().color(enable).endpoints(
            position[point1.index()].start,
            position[point2.index()].start
        )
    elif param[0] == "transistor_npn":
        point3 = createPoint(param[5], param[6])
        point3.x = int(point3.x)
        point3.y = int(point3.y)
        d += (T := elm.BjtNpn().color(enable).down().anchor(
            "emitter").at(position[point1.index()].start))
        d += elm.Line().color(enable).endpoints(
            T.base, position[point3.index()].start)
        d += elm.Line().color(enable).endpoints(
            T.collector, position[point2.index()].start)
    elif param[0] == "transistor_pnp":
        point3 = createPoint(param[5], param[6])
        point3.x = int(point3.x)
        point3.y = int(point3.y)
        d += (T := elm.BjtPnp().color(enable).down().anchor(
            "emitter").at(position[point1.index()].start).flip())
        d += elm.Line().color(enable).endpoints(
            T.base, position[point3.index()].start)
        d += elm.Line().color(enable).endpoints(
            T.collector, position[point2.index()].start)
    drawGND(d, point1, point2)
    drawVDD(d, point1, point2)


# d.draw()
d.save('./Python/CircuitDiagram.svg')
changeSVG2PNG('./Python/CircuitDiagram.svg')
d.save('C:\\Users\\iwasi\\Desktop\\研究\\IWI\\結果\\CircuitDiagram.svg')
