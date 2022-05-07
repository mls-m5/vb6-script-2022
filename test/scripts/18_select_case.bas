

Sub Main()
    Dim X as Long
    Select Case 1
        Case 0
            X = 0
        Case 1
            X = 20
    End Select

    Assert X = 20

    Select Case 1
        Case 0
            X = 0
        Case Else
            X = 30
    End Select

    Assert X = 30
End Sub