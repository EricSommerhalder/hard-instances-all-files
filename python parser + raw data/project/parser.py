import csv


def findPos(gameState, number):
    for i in range(0, 16):
        if gameState[i] == number:
            return i
    return -1


with open("output.csv", "w+", newline='') as csvfile:
    fieldnames = ["ID", "STATE", "Blank position", "Top row", "Bottom row", "Manhattan", "First column", "Last column", "Neighbours"]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()
    for jj in range(1, 101):
        if jj == 14 or jj == 17 or jj == 22 or jj == 49 or jj == 60 or jj == 82 or jj == 99:
            continue
        row = {"ID": str(jj)}
        file_path = "runs-00001-00100/00"
        if jj < 100:
            file_path += "0"
            if jj < 10:
                file_path += "0"
        file_path += str(jj) + "/run.log"
        file = open(file_path, "r")
        lines = file.readlines()
        lines = lines[-6:-2]
        gameState = []
        for i in range(0, 4):
            gameState.append(int(lines[i][1:3]))
            gameState.append(int(lines[i][4:6]))
            gameState.append(int(lines[i][7:9]))
            gameState.append(int(lines[i][10:12]))
        row["STATE"] = ', '.join(map(str, gameState))

        # 0 middle, edge or corner
        zeroPos = findPos(gameState, 0)
        if zeroPos == 0 or zeroPos == 3 or zeroPos == 12 or zeroPos == 15:
            row["Blank position"] = "corner"
        elif zeroPos < 4 or zeroPos > 11:
            row["Blank position"] = "edge"
        else:
            row["Blank position"] = "middle"

        # average top and bottom row
        avgTop = (gameState[0] + gameState[1] + gameState[2] + gameState[3]) / 4
        avgBottom = (gameState[12] + gameState[13] + gameState[14] + gameState[15]) / 4
        row["Top row"] = avgTop
        row["Bottom row"] = avgBottom

        # Manhattan distance
        totalDist = 0
        for pos in range(0, 16):
            val = gameState[pos]
            yCurr = int(pos/4)
            xCurr = pos - yCurr * 4
            yGoal = int(val/4)
            xGoal = val - yGoal * 4
            totalDist += abs(yCurr - yGoal) + abs(xCurr - xGoal)
        totalDist = totalDist / 16
        row["Manhattan"] = totalDist

        # first and last column modulo 4
        firstColumn = 0
        lastColumn = 0
        for i in range(0, 4):
            firstColumn += gameState[i * 4] % 4
            lastColumn += gameState[i * 4 + 3] % 4
        firstColumn = firstColumn / 4
        lastColumn = lastColumn / 4
        row["First column"] = firstColumn
        row["Last column"] = lastColumn

        # differences to neighbours
        differences = 0
        for i in range(0, 16):
            if not i % 4 == 0:    # left
                differences += abs(gameState[i] - gameState[i-1])
            if not i < 4:   # down
                differences += abs(gameState[i] - gameState[i - 4])
        differences = differences / 24
        row["Neighbours"] = differences
        writer.writerow(row)
        print("Done with: " + str(jj))
