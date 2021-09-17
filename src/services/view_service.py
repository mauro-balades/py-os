
"""
Simple functions to make the TUI
"""

from .console_service import ConsoleService
from .system_service import SystemService
from time import sleep

console = ConsoleService()
system = SystemService()

LOADING_CHARACTER = '▒'
LINE_CHARACTER = '─'

"""
Align text horizontaly center

:param text: Text that will be displayed
:type text: str
"""
def alignTextCenter(text: str, ret: bool = False):
  size = system.size()

  if not ret:
    console.print(text.center(size['columns']))
  else:
    return text.center(size['columns'])

"""
print one loading bar character
"""
def loadingBar(time: float = 0.01):

  print(LOADING_CHARACTER, end='', flush=True)
  sleep(time)

"""
print a fullscreen line
"""
def line():

  size = system.size()
  console.print(
    ''.join(LINE_CHARACTER for _ in range(size['columns']))
  )