
Sub Main() 
    Dim Sum as Integer
    Dim Sum2 as Integer
    For I as Integer = 1 To 10
        Sum = Sum + 1
        If I > 5 Then
            Continue For
        End If
        Sum2 = Sum2 + 1
    Next index

    Print Sum
    Print Sum2

    Assert Sum = 10
    Assert Sum2 = 5
End Sub