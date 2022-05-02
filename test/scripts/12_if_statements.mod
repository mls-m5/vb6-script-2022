

Sub Main()
    If 1
        Assert 1
    Else
        Assert 0
    End If

    If 0
        Assert 0
    ElseIf 1
        Assert 1
    End If


    If 0
        Assert 0
    ElseIf 1
        Assert 1
    Else
        Assert 0
    End If

End Sub