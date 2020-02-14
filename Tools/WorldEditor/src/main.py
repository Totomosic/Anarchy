import sys
import os
import pygame
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtCore import QSize

import scene

class ImageWidget(QtWidgets.QWidget):
    def __init__(self, surface, parent=None):
        super(ImageWidget, self).__init__(parent)
        self.width = surface.get_width()
        self.height = surface.get_height()
        self.data = surface.get_buffer().raw
        self.image = QtGui.QImage(self.data, self.width, self.height, QtGui.QImage.Format_RGB32)

    def paintEvent(self, event):
        qp = QtGui.QPainter()
        qp.begin(self)
        qp.drawImage(0, 0, self.image)
        qp.end()

class PygameWidget(ImageWidget):
    def __init__(self, surface, parent=None):
        super(PygameWidget, self).__init__(surface, parent)
        self.surface = surface
        self.scene = scene.Scene()

    def render(self):
        self.surface.fill((0, 0, 0))
        self.scene.update()
        self.scene.render(self.surface)

        pygame.draw.rect(self.surface, (255, 0, 0), (100, 100, 200, 200))

        self.data = self.surface.get_buffer().raw
        self.image = QtGui.QImage(self.data, self.width, self.height, QtGui.QImage.Format_RGB32)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self.setWindowTitle("World Editor")
        self.setMinimumSize(QSize(1280, 720))

        central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(central_widget)

        self.grid_layout = QtWidgets.QGridLayout(self)
        central_widget.setLayout(self.grid_layout)

        pygame.init()
        surface = pygame.Surface((800, 600))

        self.pygame_renderer = PygameWidget(surface, self)
        self.grid_layout.addWidget(self.pygame_renderer)

        self.update_timer = QtCore.QTimer(self)
        self.update_timer.timeout.connect(self.render)
        self.update_timer.start(1.0 / 30.0)

    def render(self):
        self.pygame_renderer.render()

app = QtWidgets.QApplication(sys.argv)
w = MainWindow()
w.show()
app.exec_()