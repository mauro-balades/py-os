from ..services.view_service import alignTextCenter
from ..services.system_service import SystemService

class BootView:

  system = SystemService()
  text = 'B O O T I N G\n'

  """
  Simple TUI view while booting  
  """
  def __init__(self):
    self.count = 0
    self.show()

  """
  add 5 to the actual count
  """
  def addCount(self):
    self.count += 5
    self.show()

  def show(self):
    self.system.clear()
    alignTextCenter(self.text)    
    alignTextCenter(str(self.count) + '%')    