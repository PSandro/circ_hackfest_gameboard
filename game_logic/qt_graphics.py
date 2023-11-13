# following tutorial https://www.pythonguis.com/pyqt5/
# PyQT5 snake game also kind of helps but it's not quite what we need
# https://www.geeksforgeeks.org/pyqt5-snake-game/
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton

# TODO use layout to make pretty window
# add "status bar" with current resources and money etc. for each player?
# send signal from button to run an iteration of the game loop?
# Qt threads might be needed

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("My App")

        button = QPushButton("Press Me!")
        button.setCheckable(True)
        button.clicked.connect(self.the_button_was_clicked)

        # Set the central widget of the Window.
        self.setCentralWidget(button)

    def the_button_was_clicked(self):
        print("Clicked!")


app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()