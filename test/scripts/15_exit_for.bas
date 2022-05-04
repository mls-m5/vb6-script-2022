
Sub Main() 
    Dim Sum as Integer
    For I as Integer = 1 To 10
        Sum = Sum + 1
        If I = 5 Then
            Exit For
        End If
    Next index

    Assert Sum = 5
End Sub