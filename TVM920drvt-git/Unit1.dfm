object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 
    'TVM920 testing debug and  standalone and OpenPNP interface . gle' +
    'n english. V0.161'
  ClientHeight = 689
  ClientWidth = 1054
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 114
    Width = 98
    Height = 13
    Caption = 'Feeder (zero based)'
  end
  object Label2: TLabel
    Left = 351
    Top = 435
    Width = 55
    Height = 13
    Caption = 'Vac toggles'
  end
  object Bevel1: TBevel
    Left = 560
    Top = 267
    Width = 137
    Height = 198
  end
  object Bevel2: TBevel
    Left = 440
    Top = 235
    Width = 114
    Height = 231
  end
  object Label3: TLabel
    Left = 456
    Top = 512
    Width = 14
    Height = 13
    Caption = 'X='
  end
  object Label4: TLabel
    Left = 456
    Top = 531
    Width = 14
    Height = 13
    Caption = 'Y='
  end
  object Label5: TLabel
    Left = 560
    Top = 513
    Width = 31
    Height = 13
    Caption = 'Label5'
  end
  object Label6: TLabel
    Left = 560
    Top = 532
    Width = 31
    Height = 13
    Caption = 'Label6'
  end
  object Label7: TLabel
    Left = 448
    Top = 579
    Width = 30
    Height = 13
    Caption = 'NZ1 ='
  end
  object Label8: TLabel
    Left = 449
    Top = 598
    Width = 30
    Height = 13
    Caption = 'NZ2 ='
  end
  object Label9: TLabel
    Left = 449
    Top = 617
    Width = 30
    Height = 13
    Caption = 'NZ3 ='
  end
  object Label10: TLabel
    Left = 449
    Top = 636
    Width = 30
    Height = 13
    Caption = 'NZ4 ='
  end
  object Label11: TLabel
    Left = 541
    Top = 579
    Width = 37
    Height = 13
    Caption = 'Label11'
  end
  object Label12: TLabel
    Left = 541
    Top = 601
    Width = 37
    Height = 13
    Caption = 'Label12'
  end
  object Label13: TLabel
    Left = 541
    Top = 622
    Width = 37
    Height = 13
    Caption = 'Label13'
  end
  object Label14: TLabel
    Left = 541
    Top = 641
    Width = 37
    Height = 13
    Caption = 'Label14'
  end
  object Label15: TLabel
    Left = 770
    Top = 561
    Width = 31
    Height = 13
    Caption = 'NA1 ='
  end
  object Label16: TLabel
    Left = 771
    Top = 580
    Width = 31
    Height = 13
    Caption = 'NA2 ='
  end
  object Label17: TLabel
    Left = 771
    Top = 599
    Width = 31
    Height = 13
    Caption = 'NA3 ='
  end
  object Label18: TLabel
    Left = 771
    Top = 618
    Width = 31
    Height = 13
    Caption = 'NA4 ='
  end
  object Label19: TLabel
    Left = 863
    Top = 561
    Width = 37
    Height = 13
    Caption = 'Label11'
  end
  object Label20: TLabel
    Left = 863
    Top = 583
    Width = 37
    Height = 13
    Caption = 'Label12'
  end
  object Label21: TLabel
    Left = 863
    Top = 604
    Width = 37
    Height = 13
    Caption = 'Label13'
  end
  object Label22: TLabel
    Left = 863
    Top = 623
    Width = 37
    Height = 13
    Caption = 'Label14'
  end
  object Bevel3: TBevel
    Left = 440
    Top = 471
    Width = 545
    Height = 202
  end
  object Bevel4: TBevel
    Left = 8
    Top = 424
    Width = 426
    Height = 183
  end
  object Bevel5: TBevel
    Left = 8
    Top = 183
    Width = 426
    Height = 235
  end
  object Label23: TLabel
    Left = 199
    Top = 334
    Width = 14
    Height = 13
    Caption = 'X='
  end
  object Label24: TLabel
    Left = 199
    Top = 353
    Width = 14
    Height = 13
    Caption = 'Y='
  end
  object Bevel6: TBevel
    Left = 711
    Top = 235
    Width = 102
    Height = 208
  end
  object Bevel7: TBevel
    Left = -106
    Top = 424
    Width = 252
    Height = 66
  end
  object Bevel8: TBevel
    Left = 832
    Top = 324
    Width = 153
    Height = 124
  end
  object Label25: TLabel
    Left = 904
    Top = 536
    Width = 23
    Height = 13
    Caption = 'degs'
  end
  object Label26: TLabel
    Left = 291
    Top = 552
    Width = 23
    Height = 13
    Caption = 'degs'
  end
  object Image1: TImage
    Left = 170
    Top = 0
    Width = 235
    Height = 146
  end
  object Button1: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'start TVM'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 8
    Top = 39
    Width = 75
    Height = 25
    Caption = 'stop TVM'
    Enabled = False
    TabOrder = 1
    OnClick = Button2Click
  end
  object Memo1: TMemo
    Left = 644
    Top = 8
    Width = 402
    Height = 211
    Lines.Strings = (
      'Memo1'
      '')
    ScrollBars = ssBoth
    TabOrder = 2
    OnDblClick = Memo1DblClick
  end
  object Button3: TButton
    Left = 454
    Top = 175
    Width = 81
    Height = 25
    Caption = 'tgl UP light'
    TabOrder = 3
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 454
    Top = 204
    Width = 81
    Height = 25
    Caption = 'tgl Down light'
    TabOrder = 4
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 351
    Top = 454
    Width = 75
    Height = 25
    Caption = 'pick1'
    TabOrder = 5
    OnClick = Button5Click
  end
  object Button6: TButton
    Left = 351
    Top = 485
    Width = 75
    Height = 25
    Caption = 'pick2'
    TabOrder = 6
    OnClick = Button6Click
  end
  object Button7: TButton
    Left = 351
    Top = 516
    Width = 75
    Height = 25
    Caption = 'pick3'
    TabOrder = 7
    OnClick = Button7Click
  end
  object Button8: TButton
    Left = 351
    Top = 547
    Width = 75
    Height = 25
    Caption = 'pick4'
    TabOrder = 8
    OnClick = Button8Click
  end
  object Button9: TButton
    Left = 8
    Top = 125
    Width = 82
    Height = 25
    Caption = 'Feed Open'
    TabOrder = 9
    OnClick = Button9Click
  end
  object Edit1: TEdit
    Left = 96
    Top = 156
    Width = 73
    Height = 21
    TabOrder = 10
    Text = '1'
  end
  object Button10: TButton
    Left = 72
    Top = 334
    Width = 57
    Height = 30
    Caption = '-Y'
    TabOrder = 11
    OnClick = Button10Click
  end
  object Button11: TButton
    Left = 72
    Top = 264
    Width = 57
    Height = 33
    Caption = '+Y'
    TabOrder = 12
    OnClick = Button11Click
  end
  object Button12: TButton
    Left = 135
    Top = 295
    Width = 50
    Height = 38
    Caption = '+X'
    TabOrder = 13
    OnClick = Button12Click
  end
  object Button13: TButton
    Left = 16
    Top = 294
    Width = 50
    Height = 41
    Caption = '-X'
    TabOrder = 14
    OnClick = Button13Click
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 670
    Width = 1054
    Height = 19
    Panels = <
      item
        Text = '0'
        Width = 50
      end
      item
        Width = 100
      end
      item
        Width = 100
      end>
  end
  object CheckBox1: TCheckBox
    Left = 576
    Top = 283
    Width = 97
    Height = 17
    Caption = '-X'
    TabOrder = 16
  end
  object CheckBox2: TCheckBox
    Left = 576
    Top = 306
    Width = 97
    Height = 17
    Caption = '+X'
    TabOrder = 17
  end
  object CheckBox3: TCheckBox
    Left = 576
    Top = 329
    Width = 97
    Height = 17
    Caption = '+Y1 (RL)'
    TabOrder = 18
  end
  object CheckBox4: TCheckBox
    Left = 576
    Top = 352
    Width = 97
    Height = 17
    Caption = '+Y2 (RR)'
    TabOrder = 19
  end
  object CheckBox5: TCheckBox
    Left = 576
    Top = 375
    Width = 97
    Height = 17
    Caption = '-Y'
    TabOrder = 20
  end
  object StaticText1: TStaticText
    Left = 576
    Top = 267
    Width = 118
    Height = 17
    Caption = 'Home / End Stop Status'
    TabOrder = 21
  end
  object CheckBox6: TCheckBox
    Left = 576
    Top = 398
    Width = 97
    Height = 17
    Caption = 'Nozz12NZ'
    TabOrder = 22
  end
  object CheckBox7: TCheckBox
    Left = 576
    Top = 421
    Width = 97
    Height = 17
    Caption = 'Nozz34NZ'
    TabOrder = 23
  end
  object CheckBox8: TCheckBox
    Left = 457
    Top = 259
    Width = 97
    Height = 17
    Caption = 'Pick1'
    TabOrder = 24
  end
  object CheckBox9: TCheckBox
    Left = 457
    Top = 282
    Width = 97
    Height = 17
    Caption = 'Pick2'
    TabOrder = 25
  end
  object CheckBox10: TCheckBox
    Left = 457
    Top = 305
    Width = 97
    Height = 17
    Caption = 'Pick3'
    TabOrder = 26
  end
  object CheckBox11: TCheckBox
    Left = 457
    Top = 328
    Width = 97
    Height = 17
    Caption = 'Pick4'
    TabOrder = 27
  end
  object CheckBox12: TCheckBox
    Left = 457
    Top = 351
    Width = 97
    Height = 17
    Caption = 'Uplight'
    TabOrder = 28
  end
  object CheckBox13: TCheckBox
    Left = 457
    Top = 374
    Width = 97
    Height = 17
    Caption = 'Downlight'
    TabOrder = 29
  end
  object StaticText2: TStaticText
    Left = 476
    Top = 236
    Width = 37
    Height = 17
    Caption = 'stats 2'
    TabOrder = 30
  end
  object Edit2: TEdit
    Left = 476
    Top = 509
    Width = 73
    Height = 21
    TabOrder = 31
    Text = 'Edit2'
  end
  object Edit3: TEdit
    Left = 476
    Top = 531
    Width = 73
    Height = 21
    TabOrder = 32
    Text = 'Edit3'
  end
  object Edit4: TEdit
    Left = 485
    Top = 577
    Width = 50
    Height = 21
    TabOrder = 33
    Text = 'Edit4'
  end
  object Edit5: TEdit
    Left = 485
    Top = 595
    Width = 50
    Height = 21
    TabOrder = 34
    Text = 'Edit5'
  end
  object Edit6: TEdit
    Left = 485
    Top = 617
    Width = 50
    Height = 21
    TabOrder = 35
    Text = 'Edit6'
  end
  object Edit7: TEdit
    Left = 485
    Top = 636
    Width = 50
    Height = 21
    TabOrder = 36
    Text = 'Edit7'
  end
  object Edit8: TEdit
    Left = 807
    Top = 559
    Width = 50
    Height = 21
    TabOrder = 37
    Text = 'Edit4'
  end
  object Edit9: TEdit
    Left = 807
    Top = 577
    Width = 50
    Height = 21
    TabOrder = 38
    Text = 'Edit5'
  end
  object Edit10: TEdit
    Left = 807
    Top = 599
    Width = 50
    Height = 21
    TabOrder = 39
    Text = 'Edit6'
  end
  object Edit11: TEdit
    Left = 807
    Top = 618
    Width = 50
    Height = 21
    TabOrder = 40
    Text = 'Edit7'
  end
  object StaticText3: TStaticText
    Left = 535
    Top = 472
    Width = 70
    Height = 17
    Caption = 'Motion Status'
    TabOrder = 41
  end
  object Button14: TButton
    Left = 57
    Top = 465
    Width = 33
    Height = 25
    Caption = 'Z1'
    TabOrder = 42
    OnClick = Button14Click
  end
  object Button15: TButton
    Left = 56
    Top = 496
    Width = 33
    Height = 25
    Caption = 'Z1'
    TabOrder = 43
    OnClick = Button15Click
  end
  object Button16: TButton
    Left = 96
    Top = 465
    Width = 33
    Height = 25
    Caption = 'Z2'
    TabOrder = 44
    OnClick = Button16Click
  end
  object Button17: TButton
    Left = 95
    Top = 496
    Width = 33
    Height = 25
    Caption = 'Z2'
    TabOrder = 45
    OnClick = Button17Click
  end
  object Button18: TButton
    Left = 136
    Top = 465
    Width = 33
    Height = 25
    Caption = 'Z3'
    TabOrder = 46
    OnClick = Button18Click
  end
  object Button19: TButton
    Left = 135
    Top = 496
    Width = 33
    Height = 25
    Caption = 'Z3'
    TabOrder = 47
    OnClick = Button19Click
  end
  object Button20: TButton
    Left = 175
    Top = 465
    Width = 33
    Height = 25
    Caption = 'Z4'
    TabOrder = 48
    OnClick = Button20Click
  end
  object Button21: TButton
    Left = 174
    Top = 496
    Width = 33
    Height = 25
    Caption = 'Z4'
    TabOrder = 49
    OnClick = Button21Click
  end
  object StaticText4: TStaticText
    Left = 33
    Top = 473
    Width = 17
    Height = 17
    Caption = 'UP'
    TabOrder = 50
  end
  object StaticText5: TStaticText
    Left = 14
    Top = 506
    Width = 36
    Height = 17
    Caption = 'DOWN'
    TabOrder = 51
  end
  object Button23: TButton
    Left = 56
    Top = 565
    Width = 33
    Height = 25
    Caption = 'A1'
    TabOrder = 52
    OnClick = Button23Click
  end
  object Button25: TButton
    Left = 95
    Top = 565
    Width = 33
    Height = 25
    Caption = 'A2'
    TabOrder = 53
    OnClick = Button25Click
  end
  object Button27: TButton
    Left = 135
    Top = 565
    Width = 33
    Height = 25
    Caption = 'A3'
    TabOrder = 54
    OnClick = Button27Click
  end
  object Button29: TButton
    Left = 174
    Top = 565
    Width = 33
    Height = 25
    Caption = 'A4'
    TabOrder = 55
    OnClick = Button29Click
  end
  object StaticText7: TStaticText
    Left = 17
    Top = 573
    Width = 33
    Height = 17
    Caption = 'ROT+'
    TabOrder = 56
  end
  object StaticText8: TStaticText
    Left = 112
    Top = 442
    Width = 31
    Height = 17
    Caption = 'HEAD'
    TabOrder = 57
  end
  object Button30: TButton
    Left = 72
    Top = 303
    Width = 56
    Height = 25
    Caption = 'SLOW'
    TabOrder = 58
    OnClick = Button30Click
  end
  object Edit12: TEdit
    Left = 217
    Top = 304
    Width = 75
    Height = 21
    TabOrder = 59
    Text = '131072'
  end
  object StaticText9: TStaticText
    Left = 217
    Top = 281
    Width = 160
    Height = 17
    Caption = 'XY tick test (multiple of 256/512)'
    TabOrder = 60
  end
  object Edit13: TEdit
    Left = 219
    Top = 331
    Width = 73
    Height = 21
    TabOrder = 61
    Text = '0'
  end
  object Edit14: TEdit
    Left = 219
    Top = 353
    Width = 73
    Height = 21
    TabOrder = 62
    Text = '0'
  end
  object CheckBox14: TCheckBox
    Left = 457
    Top = 397
    Width = 97
    Height = 17
    Caption = 'Yellow button'
    TabOrder = 63
  end
  object CheckBox15: TCheckBox
    Left = 457
    Top = 416
    Width = 97
    Height = 17
    Caption = 'Red button'
    TabOrder = 64
  end
  object CheckBox16: TCheckBox
    Left = 457
    Top = 439
    Width = 97
    Height = 17
    Caption = 'Green Button'
    TabOrder = 65
  end
  object StaticText10: TStaticText
    Left = 478
    Top = 486
    Width = 27
    Height = 17
    Caption = 'Ticks'
    TabOrder = 66
  end
  object Edit15: TEdit
    Left = 610
    Top = 509
    Width = 63
    Height = 21
    TabOrder = 67
    Text = 'Edit15'
  end
  object Edit16: TEdit
    Left = 611
    Top = 532
    Width = 62
    Height = 21
    TabOrder = 68
    Text = 'Edit16'
  end
  object StaticText11: TStaticText
    Left = 611
    Top = 486
    Width = 45
    Height = 17
    Caption = 'Distance'
    TabOrder = 69
  end
  object Button31: TButton
    Left = 679
    Top = 501
    Width = 75
    Height = 25
    Caption = 'ForceXYLoc'
    TabOrder = 70
    OnClick = Button31Click
  end
  object CheckBox17: TCheckBox
    Left = 716
    Top = 248
    Width = 97
    Height = 17
    Caption = 'Mystery1'
    TabOrder = 71
  end
  object CheckBox18: TCheckBox
    Left = 716
    Top = 271
    Width = 97
    Height = 17
    Caption = 'Mystery2'
    TabOrder = 72
  end
  object CheckBox19: TCheckBox
    Left = 716
    Top = 294
    Width = 97
    Height = 17
    Caption = 'Mystery3'
    TabOrder = 73
  end
  object Button33: TButton
    Left = 185
    Top = 613
    Width = 128
    Height = 25
    Caption = 'SetEnableYstops'
    TabOrder = 74
    OnClick = Button33Click
  end
  object Button34: TButton
    Left = 319
    Top = 613
    Width = 107
    Height = 25
    Caption = 'DisableYstop'
    TabOrder = 75
    OnClick = Button34Click
  end
  object Button35: TButton
    Left = 832
    Top = 244
    Width = 75
    Height = 25
    Caption = 'GPIO direct'
    TabOrder = 76
    OnClick = Button35Click
  end
  object Edit17: TEdit
    Left = 921
    Top = 246
    Width = 48
    Height = 21
    TabOrder = 77
    Text = '0'
  end
  object Edit18: TEdit
    Left = 975
    Top = 246
    Width = 32
    Height = 21
    TabOrder = 78
    Text = '0'
  end
  object StaticText12: TStaticText
    Left = 935
    Top = 228
    Width = 27
    Height = 17
    Caption = 'Bit #'
    TabOrder = 79
  end
  object StaticText13: TStaticText
    Left = 975
    Top = 228
    Width = 29
    Height = 17
    Caption = 'state'
    TabOrder = 80
  end
  object Button32: TButton
    Left = 22
    Top = 380
    Width = 107
    Height = 38
    Caption = 'SendStopMotion'
    TabOrder = 81
    OnClick = Button32Click
  end
  object Button36: TButton
    Left = 185
    Top = 380
    Width = 112
    Height = 25
    Caption = 'GOTOXY ticks'
    TabOrder = 82
    OnClick = Button36Click
  end
  object Edit19: TEdit
    Left = 225
    Top = 488
    Width = 48
    Height = 21
    TabOrder = 83
    Text = '500000'
  end
  object StaticText14: TStaticText
    Left = 247
    Top = 450
    Width = 65
    Height = 17
    Caption = 'Plunge value'
    TabOrder = 84
  end
  object StaticText15: TStaticText
    Left = 240
    Top = 555
    Width = 45
    Height = 17
    Caption = 'Rot ticks'
    TabOrder = 85
  end
  object Edit20: TEdit
    Left = 231
    Top = 571
    Width = 54
    Height = 21
    TabOrder = 86
    Text = '300000'
  end
  object Button37: TButton
    Left = 832
    Top = 267
    Width = 75
    Height = 25
    Caption = 'Read Registe'
    TabOrder = 87
    OnClick = Button37Click
  end
  object Edit21: TEdit
    Left = 921
    Top = 269
    Width = 64
    Height = 21
    TabOrder = 88
    Text = 'reg hex'
  end
  object Button38: TButton
    Left = 89
    Top = 8
    Width = 75
    Height = 25
    Hint = 'Start Host Interface for OPNP connection'
    Caption = 'Start Host Inf'
    TabOrder = 89
    OnClick = Button38Click
  end
  object Button39: TButton
    Left = 89
    Top = 39
    Width = 75
    Height = 25
    Caption = 'Stop Host Inf'
    Enabled = False
    TabOrder = 90
    OnClick = Button39Click
  end
  object Button40: TButton
    Left = 311
    Top = 292
    Width = 103
    Height = 33
    Caption = 'cpy machine loc in'
    TabOrder = 91
    OnClick = Button40Click
  end
  object StaticText16: TStaticText
    Left = 69
    Top = 241
    Width = 74
    Height = 17
    Caption = 'Motion Control'
    TabOrder = 92
  end
  object Edit22: TEdit
    Left = 240
    Top = 198
    Width = 97
    Height = 21
    TabOrder = 93
    Text = 'Edit22'
  end
  object Edit23: TEdit
    Left = 240
    Top = 225
    Width = 97
    Height = 21
    TabOrder = 94
    Text = 'Edit23'
  end
  object StaticText17: TStaticText
    Left = 185
    Top = 202
    Width = 54
    Height = 17
    Caption = 'X ticks/mm'
    TabOrder = 95
  end
  object StaticText18: TStaticText
    Left = 185
    Top = 225
    Width = 54
    Height = 17
    Caption = 'Y ticks/mm'
    TabOrder = 96
  end
  object Edit24: TEdit
    Left = 312
    Top = 331
    Width = 95
    Height = 21
    TabOrder = 97
    Text = '100'
  end
  object Edit25: TEdit
    Left = 311
    Top = 351
    Width = 96
    Height = 21
    TabOrder = 98
    Text = '200'
  end
  object Button41: TButton
    Left = 311
    Top = 380
    Width = 95
    Height = 25
    Caption = 'GOTOXY abs mm'
    TabOrder = 99
    OnClick = Button41Click
  end
  object CheckBox20: TCheckBox
    Left = 865
    Top = 328
    Width = 97
    Height = 17
    Caption = 'X mute'
    TabOrder = 100
    OnClick = CheckBox20Click
  end
  object CheckBox21: TCheckBox
    Left = 865
    Top = 351
    Width = 97
    Height = 17
    Caption = 'Y mute'
    TabOrder = 101
    OnClick = CheckBox21Click
  end
  object CheckBox22: TCheckBox
    Left = 865
    Top = 374
    Width = 97
    Height = 17
    Caption = 'Z mute'
    TabOrder = 102
    OnClick = CheckBox22Click
  end
  object CheckBox23: TCheckBox
    Left = 865
    Top = 397
    Width = 97
    Height = 17
    Caption = 'A mute'
    TabOrder = 103
    OnClick = CheckBox23Click
  end
  object StaticText19: TStaticText
    Left = 865
    Top = 431
    Width = 89
    Height = 17
    Caption = 'Axis Control Mute'
    TabOrder = 104
  end
  object Button42: TButton
    Left = 411
    Top = 8
    Width = 110
    Height = 95
    Caption = 'Q&D home'
    TabOrder = 105
    OnClick = Button42Click
  end
  object Button43: TButton
    Left = 343
    Top = 196
    Width = 75
    Height = 50
    Caption = 'Set scale'
    TabOrder = 106
    OnClick = Button43Click
  end
  object Button44: TButton
    Left = 527
    Top = 8
    Width = 75
    Height = 25
    Caption = 'HomeZonly'
    TabOrder = 107
    OnClick = Button44Click
  end
  object Edit26: TEdit
    Left = 600
    Top = 576
    Width = 56
    Height = 21
    TabOrder = 108
    Text = 'Edit26'
  end
  object Edit27: TEdit
    Left = 600
    Top = 597
    Width = 56
    Height = 21
    TabOrder = 109
    Text = 'Edit27'
  end
  object Edit28: TEdit
    Left = 600
    Top = 618
    Width = 56
    Height = 21
    TabOrder = 110
    Text = 'Edit28'
  end
  object Edit29: TEdit
    Left = 600
    Top = 640
    Width = 56
    Height = 21
    TabOrder = 111
    Text = 'Edit29'
  end
  object StaticText20: TStaticText
    Left = 611
    Top = 559
    Width = 20
    Height = 17
    Caption = 'mm'
    TabOrder = 112
  end
  object StaticText21: TStaticText
    Left = 496
    Top = 558
    Width = 25
    Height = 17
    Caption = 'ticks'
    TabOrder = 113
  end
  object StaticText22: TStaticText
    Left = 807
    Top = 538
    Width = 25
    Height = 17
    Caption = 'ticks'
    TabOrder = 114
  end
  object Edit30: TEdit
    Left = 279
    Top = 489
    Width = 50
    Height = 21
    TabOrder = 115
    Text = '-15'
    TextHint = '-19mm max. suggest -18.5 max'
  end
  object StaticText23: TStaticText
    Left = 292
    Top = 466
    Width = 20
    Height = 17
    Caption = 'mm'
    TabOrder = 116
  end
  object StaticText24: TStaticText
    Left = 235
    Top = 465
    Width = 25
    Height = 17
    Caption = 'ticks'
    TabOrder = 117
  end
  object CheckBox24: TCheckBox
    Left = 248
    Top = 529
    Width = 66
    Height = 17
    Caption = 'do ticks'
    TabOrder = 118
  end
  object Button45: TButton
    Left = 411
    Top = 109
    Width = 110
    Height = 25
    Caption = 'abort homing'
    TabOrder = 119
    OnClick = Button45Click
  end
  object Button46: TButton
    Left = 800
    Top = 645
    Width = 100
    Height = 25
    Caption = 'Force As to Zero'
    TabOrder = 120
    OnClick = Button46Click
  end
  object Edit31: TEdit
    Left = 906
    Top = 556
    Width = 39
    Height = 21
    TabOrder = 121
    Text = 'Edit31'
  end
  object Edit32: TEdit
    Left = 906
    Top = 580
    Width = 39
    Height = 21
    TabOrder = 122
    Text = 'Edit32'
  end
  object Edit33: TEdit
    Left = 906
    Top = 602
    Width = 39
    Height = 21
    TabOrder = 123
    Text = 'Edit33'
  end
  object Edit34: TEdit
    Left = 906
    Top = 623
    Width = 39
    Height = 21
    TabOrder = 124
    Text = 'Edit34'
  end
  object Edit35: TEdit
    Left = 291
    Top = 571
    Width = 41
    Height = 21
    TabOrder = 125
    Text = '45'
  end
  object Button22: TButton
    Left = 832
    Top = 293
    Width = 75
    Height = 25
    Caption = 'WriteReg'
    TabOrder = 126
    OnClick = Button22Click
  end
  object Edit36: TEdit
    Left = 921
    Top = 296
    Width = 64
    Height = 21
    TabOrder = 127
    Text = 'datahex'
  end
  object Button24: TButton
    Left = 8
    Top = 152
    Width = 82
    Height = 25
    Caption = 'Feed close'
    TabOrder = 128
    OnClick = Button24Click
  end
  object Button26: TButton
    Left = 185
    Top = 152
    Width = 100
    Height = 25
    Caption = 'CLoseAllFeeders'
    TabOrder = 129
    OnClick = Button26Click
  end
end
