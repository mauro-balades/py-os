import sys
from src.services.console_service import ConsoleService
from ..services.view_service import alignTextCenter, loadingBar, line
from ..services.system_service import SystemService
from inspect import getsourcefile
from getpass import getpass
from time import sleep
import os

class SetupView:

  system = SystemService()
  console = ConsoleService()
  text = 'P L E A S E  W A I T\n'


  """
  Simple TUI view while setup
  """
  def __init__(self):
    self.count = 0

    self.dir = os.path.dirname(os.path.abspath(getsourcefile(lambda:0)))
    self.assets = str(self.dir) + '/../assets'

    self.show()

  def show(self):
    self.system.clear()
    size = self.system.size()

    for _ in range(int(size['rows'] / 5)):
        self.console.print("\n")

    alignTextCenter(self.text)

    for _ in range(size['columns']):
        loadingBar()

    sleep(1)

    for i in range(2):
        self.system.clear()

        f = open(self.assets + "/text/intro.txt", "r")
        data = f.read()
        f.close()

        print("\n")
        alignTextCenter('S E T U P')
        line()

        for l in data.split('\n'):
            alignTextCenter(l)

        print("\n")
        if i == 0:
            name = sys.stdout.write(alignTextCenter('T Y P E  Y O U R  N A M E  :', ret=True))
            t = input()
        else:
            password = sys.stdout.write(alignTextCenter('T Y P E  Y O U R  P A S W O R D  :', ret=True))
            sys.stdout.flush()
            t = getpass('')