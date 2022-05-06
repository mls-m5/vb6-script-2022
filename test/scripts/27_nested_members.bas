
Structure Apa
    X as Long
End Structure

Structure Bepa
    A as Apa
End Structure

Sub Main()
    Dim B as Bepa

    B.A.X = 20
    Assert B.A.X = 20
End Sub
