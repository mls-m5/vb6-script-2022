

Function GetInt(X as Integer) as Integer
    GetInt = 1 + X
End Function

Function NoArgs() as Integer
    NoArgs = 20
End Function

Sub Main()
    Dim X as Integer

    X = GetInt(2)

    Assert X = 3

    Assert NoArgs() = 20
End Sub
