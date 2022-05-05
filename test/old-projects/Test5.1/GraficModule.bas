Attribute VB_Name = "GraficModule"
Option Explicit

Dim DX As New DirectX7
Public DD As DirectDraw7
Public PrimarySurface As DirectDrawSurface7
Public BackSurface As DirectDrawSurface7
Public ClipperObj As DirectDrawClipper

Dim PrimRect As RECT
Public BackRect As RECT
Public ScreenRect As RECT
Dim frmScreen As Form

Public Sub InitGrafic(ScreenForm As Form)
    Dim ddesk As DDSURFACEDESC2
    Set frmScreen = ScreenForm
    Set DD = DX.DirectDrawCreate("")
    DD.GetDisplayMode ddesk
    DD.SetDisplayMode 640, 400, ddesk.ddpfPixelFormat.lRGBBitCount, 0, DDSDM_STANDARDVGAMODE
    DD.SetCooperativeLevel frmScreen.hWnd, DDSCL_NORMAL
    
    With PrimRect
        .Left = 0
        .Top = 0
        .Right = 640  'Screen.Width / 15
        .Bottom = 400  'Screen.Height / 15
    End With
    BackRect = PrimRect
    With BackRect
        '.Right = 800
        '.Bottom = 600
    End With
    ScreenRect = PrimRect
    
    CreateSurfaces
End Sub

Sub CreateSurfaces()
    Dim ddsd1 As DDSURFACEDESC2
    With ddsd1
        .lFlags = DDSD_CAPS
        .ddsCaps.lCaps = DDSCAPS_PRIMARYSURFACE
    End With
    Set PrimarySurface = DD.CreateSurface(ddsd1)
    
    Dim ddsd2 As DDSURFACEDESC2
    With ddsd2
        .lFlags = DDSD_CAPS + DDSD_HEIGHT + DDSD_WIDTH
        .ddsCaps.lCaps = DDSCAPS_OFFSCREENPLAIN + DDSCAPS_SYSTEMMEMORY
        .lWidth = 640 'Screen.Width / 15
        .lHeight = 400 'Screen.Height / 15
    End With
    Set BackSurface = DD.CreateSurface(ddsd2)
    Dim key As DDCOLORKEY
    key.high = 0
    key.low = 0
    
    Set ClipperObj = DD.CreateClipper(0)
    ClipperObj.SetHWnd frmScreen.hWnd
    PrimarySurface.SetClipper ClipperObj
End Sub

Public Sub UpDateScreen()
    Dim rest As Long
    
    
    
    'rest = PrimarySurface.BltFast(1, 1, BackSurface, BackRect, DDBLTFAST_WAIT)
    rest = PrimarySurface.Blt(PrimRect, BackSurface, BackRect, DDBLT_DONOTWAIT)
    If Not (rest = 0 Or rest = DDERR_WASSTILLDRAWING) Then
        ShowError rest
        End
    End If
End Sub

Sub ShowError(num As Long)
    Select Case num
        Case DDERR_ALREADYINITIALIZED
            MsgBox "redan init"
        Case DDERR_BLTFASTCANTCLIP
            MsgBox "BLTFAST kan inte bitta"
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
        Case DDERR_SURFACEBUSY
            MsgBox "upptagen!"
        Case DDERR_WASSTILLDRAWING
            MsgBox "ritar fortfarande"
        Case Else
            If num Then MsgBox "annat"
    End Select
    If num > DDERR_ALREADYINITIALIZED And Err <= DDERR_XALIGN Then MsgBox "fel"
    If num Then End
    
End Sub

