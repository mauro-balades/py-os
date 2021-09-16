from .system_service import SystemService
from ..views.boot_view import BootView
import time

class BootService:

  default_dir = "os"
  system = SystemService()

  """
  boot the operating system
  """
  def __init__(self, dir: str = default_dir):

    self.view = BootView()
    self.dir = dir
    self.check()

  """
  checks if the operating system has been loaded correctly or not
  """
  def check(self):
    
    # TODO: remove this testing part
    for i in range(20):
      self.view.addCount()
      time.sleep(0.09)

    if not self.system.exists(self.dir):
      pass

    return True
