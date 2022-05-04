' TestImport Class08

Sub Main()
	Dim A As Class08

	' Should use "Set"
	Set A = New Class08

	A.X = 10
	A.Y = 20

	Assert A.X
	Assert A.Y
End Sub