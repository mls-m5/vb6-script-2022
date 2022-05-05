

Function OptionalFunction(Optional X as Integer = 20, Optional Y as Integer = 30)
    OptionalFunction = X + Y
End Function

Sub Main()
    Assert OptionalFunction(10, 52) = 62
    Assert OptionalFunction(, 1) = 21
    Assert OptionalFunction(22) = 52
End Sub