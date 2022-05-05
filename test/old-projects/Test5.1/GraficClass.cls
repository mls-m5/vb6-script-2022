VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "GraficClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Dim sW As Long
Dim sH As Long

Option Explicit
Private Type ImgData
    dHeight As Long
    dWidth As Long
    dRotateMax As Long
    dRowsMax As Long
    dTotalHeight As Long
    dTotalWidth As Long
End Type

Public mSurface As DirectDrawSurface7
Public MaxRotate As Long
Dim RotatePos() As RECT
Public mHeight As Long
Public mWidth As Long

Public Sub Display(X As Long, Y As Long, CurrentRotate As Byte, Height As Long, Width As Long)
    Dim dstrect As RECT
    Dim src As RECT
    Dim rest As Long
    With RotatePos(CurrentRotate)
        src.Left = .Left
        src.Top = .Top
        src.Right = .Right
        src.Bottom = .Bottom
    End With
    With dstrect
        .Left = X
        If X < 0 Then
            .Left = 0
            src.Left = -X
        End If
        .Top = Y
        If Y < 0 Then
            .Top = 0
            src.Top = -Y
        End If
        .Right = X + Width
        If .Right > sW Then
            .Right = sW
            src.Right = src.Right + (X - sW)
        End If
        .Bottom = Y + Height
        If .Bottom > sH Then
            .Bottom = sH
            src.Bottom = src.Bottom + (Y - sH)
        End If
    End With
    
    If (src.Left > src.Right - 1 Or src.Top > src.Bottom - 1) Then Exit Sub
    
    Do
        rest = BackSurface.Blt(dstrect, mSurface, src, DDBLT_DONOTWAIT Or DDBLT_KEYSRC)
        DoEvents
    Loop While rest = DDERR_WASSTILLDRAWING
    'If rest Then ShowError rest
End Sub

Public Sub FastDisplay(ByVal X&, ByVal Y&, RotateNum As Long)
    Dim rest&
    Dim src As RECT
    Dim nX As Long
    Dim nY As Long
    nX = X
    nY = Y
    With src
        .Left = RotatePos(RotateNum).Left
        .Right = RotatePos(RotateNum).Right
        If X < 0 Then
            src.Left = .Left - X
            nX = 0
        ElseIf X + mWidth > sW Then
            .Right = .Right + (sW - mWidth - X)
        End If
        .Top = RotatePos(RotateNum).Top
        .Bottom = RotatePos(RotateNum).Bottom
        If Y < 0 Then
            nY = 0
            .Top = .Top - Y
        ElseIf Y + mHeight > sH Then
            .Bottom = .Bottom + (sH - mHeight - Y)
        End If
    End With
    rest = BackSurface.BltFast(nX, nY, mSurface, src, DDBLTFAST_WAIT Or DDBLTFAST_SRCCOLORKEY)
End Sub

Public Sub DDLine(X1 As Long, Y1 As Long, X2 As Long, Y2 As Long, DrawWidth As Long, Color As Long)
    BackSurface.SetForeColor Color
    BackSurface.setDrawWidth DrawWidth
    BackSurface.DrawLine X1, Y1, X2, Y2
End Sub

Public Sub LoadGrafic(GraficPath As String)
    Dim NData As String, NData2$
    Dim GData As ImgData
    Dim CurrentRot As Long
    
    Open (GraficPath & ".txt") For Input As #1
    Do While Not EOF(1)
        Input #1, NData, NData2
        With GData
        Select Case LCase(NData)
            Case "[width]"
                .dWidth = NData2
                mWidth = .dWidth
            Case "[height]"
                .dHeight = NData2
                mHeight = .dHeight
            Case "[max rotation]"
                .dRotateMax = NData2 - 1
                MaxRotate = .dRotateMax
            Case "[max rows]"
                .dRowsMax = NData2 - 1
            Case "[total height]"
                .dTotalHeight = NData2
            Case "[total width]"
                .dTotalWidth = NData2
        End Select
        End With
    Loop
    Close #1
    
    Dim ddsd As DDSURFACEDESC2
    Dim i As Long
    Dim X As Long
    Dim Y As Long
    With ddsd
        .lFlags = DDSD_CAPS '+ DDSD_HEIGHT + DDSD_WIDTH
        .ddsCaps.lCaps = DDSCAPS_OFFSCREENPLAIN
        '.lWidth = GData.dTotalWidth
        '.lHeight = GData.dTotalHeight
    End With
    
    If (Dir(GraficPath & ".bmp")) = "" Then
        MsgBox "finns inte"
        Exit Sub
    End If
    On Error GoTo ErrorHandler
    
    Set mSurface = DD.CreateSurfaceFromFile(GraficPath & ".bmp", ddsd)
    
    
    Dim key As DDCOLORKEY
    key.low = 0
    key.high = 0
    mSurface.SetColorKey DDCKEY_SRCBLT, key
    
    ReDim RotatePos(GData.dRotateMax)
    For i = 0 To GData.dRotateMax
        With RotatePos(i)
            .Left = X
            .Top = Y
            .Right = X + GData.dWidth
            .Bottom = Y + GData.dHeight
        End With
        CurrentRot = CurrentRot + 1
        X = X + GData.dWidth
        If CurrentRot >= GData.dRotateMax Then
            CurrentRot = 0
            X = 0
            Y = Y + GData.dHeight
        End If
    Next
    Exit Sub
ErrorHandler:
    ShowError Err.Number
End Sub


Sub ShowError(num As Long)
    Select Case num
        Case DDERR_ALREADYINITIALIZED
            MsgBox "redan init"
        Case DDERR_BLTFASTCANTCLIP
        Case DDERR_CANNOTATTACHSURFACE
        Case DDERR_CANTLOCKSURFACE
            MsgBox "kan inte fixera"
        Case DDERR_COLORKEYNOTSET
            MsgBox "colorkey"
        Case DDERR_DCALREADYCREATED
            MsgBox "redan gjord"
        Case DDERR_INVALIDRECT
            MsgBox "rect"
        Case DDERR_INVALIDPARAMS
            MsgBox "param"
        Case DDERR_CANTCREATEDC
            MsgBox "kan inte göra yta"
        Case DDERR_DEVICEDOESNTOWNSURFACE
            MsgBox "ingen suport"
        Case DDERR_UNSUPPORTEDFORMAT
            MsgBox "fel format"
        Case DDERR_INVALIDCAPS
            MsgBox "fel caps"
        Case DDERR_NOCOOPERATIVELEVELSET
            MsgBox "ingen cl"
        Case DDERR_NONONLOCALVIDMEM
            MsgBox "inget minne"
        Case DDERR_NOTFOUND
            MsgBox "hittar inte"
        Case Is < DDERR_TOOBIGHEIGHT
            MsgBox "mer"
        Case Else
            MsgBox "annat"
    End Select
    If num > DDERR_ALREADYINITIALIZED And Err <= DDERR_XALIGN Then MsgBox "fel"
    If num Then End
    
End Sub

Private Sub Class_Initialize()
    sW = ScreenRect.Right
    sH = ScreenRect.Bottom
End Sub
