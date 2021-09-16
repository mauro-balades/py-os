import os

"""
Everything that includes the OS will happen here
"""
class SystemService:

  """
  checks if a directory exists

  :param directory: The directory that needs to be checked
  :type directory: str
  :return: True | False
  """
  def exists(self, directory: str):
    return os.path.exists(directory)

  """
  get terminal's size
  """
  def size(self):
    size = os.get_terminal_size()

    return {
      "columns": size.columns,
      "rows": size.lines
    }

  """
  clear the terminal
  """
  def clear(self):
    os.system('cls' if os.name == 'nt' else 'clear')

