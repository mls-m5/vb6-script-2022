
Dim X as Integer
Dim W as Integer

Sub Test(Y as Integer, Z as Integer)
	X = Y
	W = Z
End Sub

Sub Main()
    Test 10, 20

	Assert X
	Assert W
End Sub