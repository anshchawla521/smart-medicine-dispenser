lines = ""

with open('index.html', 'r') as file:
    lines = file.readlines()
with open('index1.html', 'w+') as file:
    for line in lines:
        if line[:-1].replace(" ", "") == "":
            continue
        if '''"''' in line :
            print(line)
            line = line.replace('''"''' , '\\\"')
        file.write(line[:-1] + '\\\n')
