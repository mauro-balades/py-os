from .system_service import SystemService
from ..views.setup_view import SetupView

class SetupService:

  default_dir = "os"
  system = SystemService()

  """
  setup the operating system
  """
  def __init__(self, dir: str = default_dir):

    self.view = SetupView()
    self.dir = dir
    self.execute()

  """
  setup with UI
  """
  def execute(self):
    size = self.system.size()
    self.system.clear()

    return True
