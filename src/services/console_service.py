from termcolor import cprint

"""
A service to make outputing easy
"""
class ConsoleService:

  error_prefix = '[ERROR]: '

  """
  print text to the terminal

  :param text*: text to be displayed
  :param color: output with color if needed
  :type text: str
  :type color: str

  >> console.print('text', 'red')
  """
  def print(self, text: str, color: str = ""):

    if color != "":
      cprint(text, color)
      return
    
    print(text)

  """
  print an error on the terminal

  :param text: text to be displayed
  :type text: str
  """
  def error(self, text: str):
    cprint(self.error_prefix + text, 'red')