from termcolor import cprint

"""
A service to make outputing easy
"""
class ConsoleService:

  """
  print text to the terminal

  :param text*: text to be displayed
  :param color: output with color if needed
  :type text: str 
  :type color: str

  >> console.print('text', 'red')
  """
  def print(text: str, color: str = ""):

    if color != "":
      cprint(text, color)
      return
    
    print(text)