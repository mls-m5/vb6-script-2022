
Structure Apa
	X as Integer
	Y as Integer
End Structure

Sub Main()
	Dim A as Apa
	Dim B as Apa

	A.X = 20
	A.Y = 30

	B = A

	B.X = A.X

	Assert A.X
	Assert A.Y
End Sub