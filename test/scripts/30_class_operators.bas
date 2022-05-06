' TestImport Class30

Sub Main()
    Dim C as Class30
    Dim D as Class30

    ' Assert C Is Nothing

    Set C = New Class30

    Assert Not C Is Nothing

    Set D = New Class30

    Assert Not C Is D
    Set D = C
    Assert C Is D
End Sub