
Structure Apa
	X as Integer
	Y as Integer
End Structure

Sub Test(A as Apa)
	Assert A.X
	Assert A.Y

	A.X = 0
End Sub

Sub Main()
	Dim A as Apa

	A.X = 20
	A.Y = 30

	Test A
End Sub