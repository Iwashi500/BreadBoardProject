from tkinter import *
from tkinter import ttk
import ChangeSVG2PNG
import schemdraw
import schemdraw.elements as elm
import math
import pandas as pd
import time
import csv


# パラメータ
disable = 'white'
enable = 'black'
svg_filename = 'Python/CircuitDiagram.svg'
image_tag = "circuit_image"
image_width = 640
image_height = 800
sample_path = "Python/sample-parameter.csv"
csv_header = ['type', 'point1_x', 'point1_y', 'point2_x', 'point2_y']
circuit_pos = 1
circuit_row = 1
# 回路制作
d = schemdraw.Drawing(unit=1.0, inches_per_unit=0.5)
d += (P := elm.Line().down().color(disable))
d += elm.VDD().color(enable).right
# ダイアログ
root = Tk()
root.title('自作回路作成機')
main_frame = ttk.Frame(root, padding=16)
image = PhotoImage(file="Python/CircuitDiagram.png")
canvas = Canvas(main_frame, width=image_width, height=image_height, bd=0,
                highlightthickness=0, relief='ridge')
# csvファイル
file = open(sample_path, 'w', newline='')
csv_writer = csv.writer(file)
csv_writer.writerow(csv_header)


def create_parameter(type):
    global circuit_pos
    res = [type, circuit_row, circuit_pos, circuit_row, circuit_pos + 1]
    circuit_pos += 1
    return res


def update_image():
    global image
    canvas.delete(image_tag)
    image = PhotoImage(file="Python/CircuitDiagram.png")
    canvas.create_image(image_width / 2, image_height / 2,
                        image=image, tag=image_tag)


def updateCircuit():
    global d
    d.save(svg_filename)
    ChangeSVG2PNG.changeSVG2PNG(svg_filename)


def click_resistor():
    global d
    d += elm.Resistor().color(enable).down()
    updateCircuit()
    update_image()
    csv_writer.writerow(create_parameter('resistor'))


def click_LED():
    global d
    d += elm.LED().color(enable).down()
    updateCircuit()
    update_image()
    csv_writer.writerow(create_parameter('LED'))


def click_condensor():
    global d
    d += elm.Capacitor().color(enable).down()
    updateCircuit()
    update_image()
    csv_writer.writerow(create_parameter('condenser'))


def click_end():
    global d
    d += elm.GND().color(enable).right()
    updateCircuit()
    update_image()
    csv_writer.writerow(['wire', circuit_row, circuit_pos, circuit_row, 12])
    file.close()


# ウィジェットの作成
button_frame = ttk.Frame(main_frame, padding=8)
button_resistor = ttk.Button(
    button_frame,
    text='Resistor',
    command=lambda: click_resistor())
button_LED = ttk.Button(
    button_frame,
    text='LED',
    command=lambda: click_LED())
button_condensor = ttk.Button(
    button_frame,
    text='Condensor',
    command=lambda: click_condensor())
button_end = ttk.Button(
    button_frame,
    text='GND(End)',
    command=lambda: click_end())
canvas.create_image(image_width / 2, image_height / 2,
                    image=image, tag=image_tag)
# レイアウト
pady = 10
main_frame.pack()
canvas.pack(side=LEFT)
button_frame.pack(side=LEFT)
button_resistor.pack(pady=pady)
button_LED.pack(pady=pady)
button_condensor.pack(pady=pady)
button_end.pack(pady=pady)

updateCircuit()
update_image()

# ウィンドウの表示開始
root.mainloop()
