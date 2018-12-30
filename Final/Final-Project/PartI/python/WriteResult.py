import xlwt
import numpy as np

def writeExcelResult(path, flag):
	result = xlwt.Workbook(encoding='utf-8')
	sheet = result.add_sheet('Sheet 1', cell_overwrite_ok=True)
	# Write header
	header = [
		"Image Name", "Corner Point 1", "Corner Point 2", "Corner Point 3",
		"Corner Point 4", "Student ID Number", "Phone Number", "Personal ID Number"
	]
	for i in range(len(header)):
		sheet.write(0, i, header[i])
	# Read result txt datas into xlsx datas
	srcPath = "../temp0/img"
	length = 10
	if flag :
		srcPath = "../temp/img"

	rawNumber = 1
	for index in range(length):
		# Write Image Number
		imgName = str(index + 1) + ".bmp"
		sheet.write(rawNumber, 0, imgName)
		colNumber = 1
		# Write corner points result
		targetPath = srcPath + str(index+1) + "/points.txt"
		reader = np.loadtxt(targetPath, dtype=str)
		for j in range(len(reader)):
			dstStr = "(" + str(reader[j][0]) + ", " + str(reader[j][1]) + ")"
			sheet.write(rawNumber, colNumber, dstStr)
			colNumber += 1

		# Write Number results
		targetpath = srcPath + str(index+1) + "/detect.txt"
		reader = np.loadtxt(targetpath, dtype=str)
		for j in range(len(reader)):
			if j > 2 and j % 3 == 0:
				rawNumber += 1
				colNumber = 5
				sheet.write(rawNumber, 0, imgName)
			targetStr = reader[j]
			sheet.write(rawNumber, colNumber, targetStr)
			colNumber += 1
		rawNumber += 1

	result.save(path)


if __name__ == '__main__':
	path = "../output/result.xlsx"
	writeExcelResult(path=path, flag=True)