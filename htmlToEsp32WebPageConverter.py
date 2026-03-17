from tkinter import *
from tkinter import filedialog

def openFile():
    filepath=filedialog.askopenfilename(title="Válasz fájlt",
                                        filetypes=(("text files","*.txt"),
                                                   ("webpages","*.html"),
                                                   ("all files","*.*")))
    with open(filepath, "r", encoding="utf-8") as file:
        sorok = file.read().splitlines()
    file.close()
    with open("newfile.txt", "w", encoding="utf-8") as newfile:
        for i, sor in enumerate(sorok):
            sor = sor.strip()
            if i == 0:
                elsosor = f'String html = "{sor}";'
                newfile.write(elsosor + "\n")
            else:
                msor = f'html += "{sor}";'
                newfile.write(msor + "\n")
    newfile.close()

window =Tk()
button=Button(text="Válasz fájlt",command=openFile)
button.pack()
window.mainloop()


    
    
    

