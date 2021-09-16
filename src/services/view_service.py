
"""
Simple functions to make the TUI
"""

from .console_service import ConsoleService
from .system_service import SystemService

console = ConsoleService()
system = SystemService()

"""
Align text horizontaly center

:param text: Text that will be displayed
:type text: str
"""
def alignTextCenter(text: str):
  size = system.size()
  print(text.center(size['columns']))  