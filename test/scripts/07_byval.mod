
Structure Apa
	X as Integer
	Y as Integer
End Structure

Sub Test(ByVal A as Apa)

	Assert A.X
	Assert A.Y

	A.X = 0

End Sub

Sub TestTwo(ByRef A as Apa)
	A.X = 1
End Sub

Sub Main()
	Dim A as Apa

	A.X = 20
	A.Y = 30

	Test A

	Assert A.X ' Test that the struct was copied and the original is unchanged

	A.X = 0
	TestTwo A

	Assert A.X

End Sub