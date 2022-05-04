

Sub Main()
    Dim X as Integer
    X = 0

    If 1 Then
        Assert 1
        X = 1
    Else
        Assert 0
    End If

    Assert X

    If 0 Then
        Assert 0
    ElseIf 1 Then
        Assert 1
    End If


    If 0 Then
        Assert 0
    ElseIf 1 Then
        Assert 1
    Else
        Assert 0
    End If

End Sub