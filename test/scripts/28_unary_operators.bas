
Sub Main()
    Dim X as Long

    Assert -9 = 0 - 9
    Assert 0 - 9 = -9

    X = -9
    Assert X = 0 - 9
    Assert 0 - 9 = X

    Assert 9 = 10 + -1

    Print "Done"
End Sub