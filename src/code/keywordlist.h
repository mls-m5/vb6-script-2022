#pragma once

// #Const)
//  TOKEN_KEYWORD(#If…Then…#Else)
//  TOKEN_KEYWORD(&)
//  TOKEN_KEYWORD(&=)
//  TOKEN_KEYWORD(*)
//  TOKEN_KEYWORD(*=)
//  TOKEN_KEYWORD(/)
//  TOKEN_KEYWORD(/=)
//  TOKEN_KEYWORD(\)
//  TOKEN_KEYWORD(\=)
//  TOKEN_KEYWORD(^^=)
//  TOKEN_KEYWORD(+)
//  TOKEN_KEYWORD(+=)
//  TOKEN_KEYWORD(=)
//  TOKEN_KEYWORD(-=)

// http://www.yaldex.com/vb-net-tutorial-2/library.books24x7.com/book/id_5526/viewer.asp@bookid=5526&chunkid=0941343935.htm
#define TOKEN_KEYWORD_LIST                                                     \
    TOKEN_KEYWORD(Add)                                                         \
    TOKEN_KEYWORD(AddHandler)                                                  \
    TOKEN_KEYWORD(AddressOf)                                                   \
    TOKEN_KEYWORD(Alias)                                                       \
    TOKEN_KEYWORD(And)                                                         \
    TOKEN_KEYWORD(AndAlso)                                                     \
    TOKEN_KEYWORD(Ansi)                                                        \
    TOKEN_KEYWORD(AppActivate)                                                 \
    TOKEN_KEYWORD(As)                                                          \
    TOKEN_KEYWORD(Asc)                                                         \
    TOKEN_KEYWORD(AscW)                                                        \
    TOKEN_KEYWORD(Assembly)                                                    \
    TOKEN_KEYWORD(Auto)                                                        \
    TOKEN_KEYWORD(Beep)                                                        \
    TOKEN_KEYWORD(Boolean)                                                     \
    TOKEN_KEYWORD(ByRef)                                                       \
    TOKEN_KEYWORD(Byte)                                                        \
    TOKEN_KEYWORD(ByVal)                                                       \
    TOKEN_KEYWORD(Call)                                                        \
    TOKEN_KEYWORD(CallByName)                                                  \
    TOKEN_KEYWORD(Case)                                                        \
    TOKEN_KEYWORD(Catch)                                                       \
    TOKEN_KEYWORD(CBool)                                                       \
    TOKEN_KEYWORD(CByte)                                                       \
    TOKEN_KEYWORD(CChar)                                                       \
    TOKEN_KEYWORD(CDate)                                                       \
    TOKEN_KEYWORD(CDbl)                                                        \
    TOKEN_KEYWORD(CDec)                                                        \
    TOKEN_KEYWORD(Char)                                                        \
    TOKEN_KEYWORD(ChDir)                                                       \
    TOKEN_KEYWORD(ChDrive)                                                     \
    TOKEN_KEYWORD(Choose)                                                      \
    TOKEN_KEYWORD(Chr)                                                         \
    TOKEN_KEYWORD(CInt)                                                        \
    TOKEN_KEYWORD(Class)                                                       \
    TOKEN_KEYWORD(Clear)                                                       \
    TOKEN_KEYWORD(CLng)                                                        \
    TOKEN_KEYWORD(Close)                                                       \
    TOKEN_KEYWORD(CObj)                                                        \
    TOKEN_KEYWORD(Command)                                                     \
    TOKEN_KEYWORD(Continue) /* was not included from the beginning */          \
    TOKEN_KEYWORD(Const)                                                       \
    TOKEN_KEYWORD(Count)                                                       \
    TOKEN_KEYWORD(CreateObject)                                                \
    TOKEN_KEYWORD(CShort)                                                      \
    TOKEN_KEYWORD(CSng)                                                        \
    TOKEN_KEYWORD(CStr)                                                        \
    TOKEN_KEYWORD(CType)                                                       \
    TOKEN_KEYWORD(CurDir)                                                      \
    TOKEN_KEYWORD(Date)                                                        \
    TOKEN_KEYWORD(DateAdd)                                                     \
    TOKEN_KEYWORD(DateDiff)                                                    \
    TOKEN_KEYWORD(DatePart)                                                    \
    TOKEN_KEYWORD(DateSerial)                                                  \
    TOKEN_KEYWORD(DateString)                                                  \
    TOKEN_KEYWORD(DateValue)                                                   \
    TOKEN_KEYWORD(Day)                                                         \
    TOKEN_KEYWORD(DDB)                                                         \
    TOKEN_KEYWORD(Decimal)                                                     \
    TOKEN_KEYWORD(Declare)                                                     \
    TOKEN_KEYWORD(Default)                                                     \
    TOKEN_KEYWORD(Delegate)                                                    \
    TOKEN_KEYWORD(DeleteSetting)                                               \
    TOKEN_KEYWORD(Description)                                                 \
    TOKEN_KEYWORD(Dim)                                                         \
    TOKEN_KEYWORD(Dir)                                                         \
    TOKEN_KEYWORD(Do)                                                          \
    TOKEN_KEYWORD(Double)                                                      \
    TOKEN_KEYWORD(Each)                                                        \
    TOKEN_KEYWORD(Else)                                                        \
    TOKEN_KEYWORD(ElseIf)                                                      \
    TOKEN_KEYWORD(End)                                                         \
    TOKEN_KEYWORD(Enum)                                                        \
    TOKEN_KEYWORD(Environ)                                                     \
    /*TOKEN_KEYWORD(EOF)*/                                                     \
    TOKEN_KEYWORD(Erase)                                                       \
    TOKEN_KEYWORD(Erl)                                                         \
    TOKEN_KEYWORD(Err)                                                         \
    TOKEN_KEYWORD(Error)                                                       \
    TOKEN_KEYWORD(ErrorToString)                                               \
    TOKEN_KEYWORD(Event)                                                       \
    TOKEN_KEYWORD(Exit)                                                        \
    TOKEN_KEYWORD(ExternalSource)                                              \
    TOKEN_KEYWORD(False)                                                       \
    TOKEN_KEYWORD(FileAttr)                                                    \
    TOKEN_KEYWORD(FileCopy)                                                    \
    TOKEN_KEYWORD(FileDateTime)                                                \
    TOKEN_KEYWORD(FileGet)                                                     \
    TOKEN_KEYWORD(FileLen)                                                     \
    TOKEN_KEYWORD(FileOpen)                                                    \
    TOKEN_KEYWORD(FilePut)                                                     \
    TOKEN_KEYWORD(FileWidth)                                                   \
    TOKEN_KEYWORD(Filter)                                                      \
    TOKEN_KEYWORD(Finally)                                                     \
    TOKEN_KEYWORD(Fix)                                                         \
    TOKEN_KEYWORD(For)                                                         \
    TOKEN_KEYWORD(FormatCurrency)                                              \
    TOKEN_KEYWORD(FormatDateTime)                                              \
    TOKEN_KEYWORD(FormatNumber)                                                \
    TOKEN_KEYWORD(FormatPercent)                                               \
    TOKEN_KEYWORD(FreeFile)                                                    \
    TOKEN_KEYWORD(Friend)                                                      \
    TOKEN_KEYWORD(Function)                                                    \
    TOKEN_KEYWORD(FV)                                                          \
    TOKEN_KEYWORD(Get)                                                         \
    TOKEN_KEYWORD(GetAllSettings)                                              \
    TOKEN_KEYWORD(GetAttr)                                                     \
    TOKEN_KEYWORD(GetChar)                                                     \
    TOKEN_KEYWORD(GetException)                                                \
    TOKEN_KEYWORD(GetObject)                                                   \
    TOKEN_KEYWORD(GetSetting)                                                  \
    TOKEN_KEYWORD(GetType)                                                     \
    TOKEN_KEYWORD(GoTo)                                                        \
    TOKEN_KEYWORD(Handles)                                                     \
    TOKEN_KEYWORD(HelpContext)                                                 \
    TOKEN_KEYWORD(HelpFile)                                                    \
    TOKEN_KEYWORD(Hex)                                                         \
    TOKEN_KEYWORD(Hour)                                                        \
    TOKEN_KEYWORD(If)                                                          \
    TOKEN_KEYWORD(IIf)                                                         \
    TOKEN_KEYWORD(Implements)                                                  \
    TOKEN_KEYWORD(Imports)                                                     \
    TOKEN_KEYWORD(In)                                                          \
    TOKEN_KEYWORD(Inherits)                                                    \
    TOKEN_KEYWORD(Input)                                                       \
    TOKEN_KEYWORD(InputBox)                                                    \
    TOKEN_KEYWORD(InputString)                                                 \
    TOKEN_KEYWORD(InStr)                                                       \
    TOKEN_KEYWORD(InStrRev)                                                    \
    TOKEN_KEYWORD(Int)                                                         \
    TOKEN_KEYWORD(Integer)                                                     \
    TOKEN_KEYWORD(Interface)                                                   \
    TOKEN_KEYWORD(IPmt)                                                        \
    TOKEN_KEYWORD(IRR)                                                         \
    TOKEN_KEYWORD(Is)                                                          \
    TOKEN_KEYWORD(IsArray)                                                     \
    TOKEN_KEYWORD(IsDate)                                                      \
    TOKEN_KEYWORD(IsDBNull)                                                    \
    TOKEN_KEYWORD(IsError)                                                     \
    TOKEN_KEYWORD(IsNothing)                                                   \
    TOKEN_KEYWORD(IsNumeric)                                                   \
    TOKEN_KEYWORD(IsReference)                                                 \
    TOKEN_KEYWORD(Item)                                                        \
    TOKEN_KEYWORD(Join)                                                        \
    TOKEN_KEYWORD(Kill)                                                        \
    TOKEN_KEYWORD(LastDllError)                                                \
    TOKEN_KEYWORD(LBound)                                                      \
    TOKEN_KEYWORD(LCase)                                                       \
    TOKEN_KEYWORD(Left)                                                        \
    TOKEN_KEYWORD(Len)                                                         \
    TOKEN_KEYWORD(Let)                                                         \
    TOKEN_KEYWORD(Lib)                                                         \
    TOKEN_KEYWORD(Like)                                                        \
    TOKEN_KEYWORD(LineInput)                                                   \
    TOKEN_KEYWORD(Loc)                                                         \
    TOKEN_KEYWORD(Lock)                                                        \
    TOKEN_KEYWORD(LOF)                                                         \
    TOKEN_KEYWORD(Long)                                                        \
    TOKEN_KEYWORD(Loop)                                                        \
    TOKEN_KEYWORD(LSet)                                                        \
    TOKEN_KEYWORD(LTrim)                                                       \
    TOKEN_KEYWORD(Me)                                                          \
    TOKEN_KEYWORD(Mid)                                                         \
    TOKEN_KEYWORD(Minute)                                                      \
    TOKEN_KEYWORD(MIRR)                                                        \
    TOKEN_KEYWORD(MkDir)                                                       \
    TOKEN_KEYWORD(Mod)                                                         \
    TOKEN_KEYWORD(Module)                                                      \
    TOKEN_KEYWORD(Month)                                                       \
    TOKEN_KEYWORD(MonthName)                                                   \
    TOKEN_KEYWORD(MsgBox)                                                      \
    TOKEN_KEYWORD(MustInherit)                                                 \
    TOKEN_KEYWORD(MustOverride)                                                \
    TOKEN_KEYWORD(MyBase)                                                      \
    TOKEN_KEYWORD(MyClass)                                                     \
    TOKEN_KEYWORD(Namespace)                                                   \
    TOKEN_KEYWORD(New)                                                         \
    TOKEN_KEYWORD(Next)                                                        \
    TOKEN_KEYWORD(Not)                                                         \
    TOKEN_KEYWORD(Nothing)                                                     \
    TOKEN_KEYWORD(NotInheritable)                                              \
    TOKEN_KEYWORD(NotOverridable)                                              \
    TOKEN_KEYWORD(Now)                                                         \
    TOKEN_KEYWORD(NPer)                                                        \
    TOKEN_KEYWORD(NPV)                                                         \
    TOKEN_KEYWORD(Number)                                                      \
    TOKEN_KEYWORD(Object)                                                      \
    TOKEN_KEYWORD(Oct)                                                         \
    TOKEN_KEYWORD(On)                                                          \
    TOKEN_KEYWORD(Option)                                                      \
    TOKEN_KEYWORD(Optional)                                                    \
    TOKEN_KEYWORD(Or)                                                          \
    TOKEN_KEYWORD(OrElse)                                                      \
    TOKEN_KEYWORD(Overloads)                                                   \
    TOKEN_KEYWORD(Overridable)                                                 \
    TOKEN_KEYWORD(Overrides)                                                   \
    TOKEN_KEYWORD(ParamArray)                                                  \
    TOKEN_KEYWORD(Partition)                                                   \
    TOKEN_KEYWORD(Pmt)                                                         \
    TOKEN_KEYWORD(PPmt)                                                        \
    TOKEN_KEYWORD(Preserve)                                                    \
    TOKEN_KEYWORD(Print)                                                       \
    TOKEN_KEYWORD(PrintLine)                                                   \
    TOKEN_KEYWORD(Private)                                                     \
    TOKEN_KEYWORD(Property)                                                    \
    TOKEN_KEYWORD(Protected)                                                   \
    TOKEN_KEYWORD(Public)                                                      \
    TOKEN_KEYWORD(PV)                                                          \
    TOKEN_KEYWORD(QBColor)                                                     \
    TOKEN_KEYWORD(Raise)                                                       \
    TOKEN_KEYWORD(RaiseEvent)                                                  \
    TOKEN_KEYWORD(Randomize)                                                   \
    TOKEN_KEYWORD(Rate)                                                        \
    TOKEN_KEYWORD(ReadOnly)                                                    \
    TOKEN_KEYWORD(ReDim)                                                       \
    TOKEN_KEYWORD(Region)                                                      \
    TOKEN_KEYWORD(Rem)                                                         \
    TOKEN_KEYWORD(Remove)                                                      \
    TOKEN_KEYWORD(RemoveHandler)                                               \
    TOKEN_KEYWORD(Rename)                                                      \
    TOKEN_KEYWORD(Replace)                                                     \
    TOKEN_KEYWORD(Reset)                                                       \
    TOKEN_KEYWORD(Resume)                                                      \
    TOKEN_KEYWORD(Return)                                                      \
    TOKEN_KEYWORD(RGB)                                                         \
    TOKEN_KEYWORD(RmDir)                                                       \
    TOKEN_KEYWORD(Rnd)                                                         \
    TOKEN_KEYWORD(RSet)                                                        \
    TOKEN_KEYWORD(RTrim)                                                       \
    TOKEN_KEYWORD(SaveSetting)                                                 \
    TOKEN_KEYWORD(Seek)                                                        \
    TOKEN_KEYWORD(ScriptEngine)                                                \
    TOKEN_KEYWORD(Second)                                                      \
    TOKEN_KEYWORD(BuildVersion)                                                \
    TOKEN_KEYWORD(MajorVersion)                                                \
    TOKEN_KEYWORD(MinorVersion)                                                \
    TOKEN_KEYWORD(Select)                                                      \
    TOKEN_KEYWORD(Set)                                                         \
    TOKEN_KEYWORD(SetAttr)                                                     \
    TOKEN_KEYWORD(Shadows)                                                     \
    TOKEN_KEYWORD(Shared)                                                      \
    TOKEN_KEYWORD(Shell)                                                       \
    TOKEN_KEYWORD(Short)                                                       \
    TOKEN_KEYWORD(Single)                                                      \
    TOKEN_KEYWORD(SLN)                                                         \
    TOKEN_KEYWORD(Source)                                                      \
    TOKEN_KEYWORD(Space)                                                       \
    TOKEN_KEYWORD(Spc)                                                         \
    TOKEN_KEYWORD(Split)                                                       \
    TOKEN_KEYWORD(Static)                                                      \
    TOKEN_KEYWORD(Step)                                                        \
    TOKEN_KEYWORD(Stop)                                                        \
    TOKEN_KEYWORD(Str)                                                         \
    TOKEN_KEYWORD(StrComp)                                                     \
    TOKEN_KEYWORD(StrConv)                                                     \
    TOKEN_KEYWORD(StrDup)                                                      \
    TOKEN_KEYWORD(String)                                                      \
    TOKEN_KEYWORD(StrReverse)                                                  \
    TOKEN_KEYWORD(Structure)                                                   \
    TOKEN_KEYWORD(Sub)                                                         \
    TOKEN_KEYWORD(Switch)                                                      \
    TOKEN_KEYWORD(SYD)                                                         \
    TOKEN_KEYWORD(SyncLock)                                                    \
    TOKEN_KEYWORD(SystemTypeName)                                              \
    TOKEN_KEYWORD(Tab)                                                         \
    TOKEN_KEYWORD(Then)                                                        \
    TOKEN_KEYWORD(Throw)                                                       \
    TOKEN_KEYWORD(TimeOfDay)                                                   \
    TOKEN_KEYWORD(Timer)                                                       \
    TOKEN_KEYWORD(TimeSerial)                                                  \
    TOKEN_KEYWORD(TimeString)                                                  \
    TOKEN_KEYWORD(TimeValue)                                                   \
    TOKEN_KEYWORD(To)                                                          \
    TOKEN_KEYWORD(Today)                                                       \
    TOKEN_KEYWORD(Trim)                                                        \
    TOKEN_KEYWORD(True)                                                        \
    TOKEN_KEYWORD(Try)                                                         \
    TOKEN_KEYWORD(TypeName)                                                    \
    TOKEN_KEYWORD(TypeOf)                                                      \
    TOKEN_KEYWORD(UBound)                                                      \
    TOKEN_KEYWORD(UCase)                                                       \
    TOKEN_KEYWORD(Unicode)                                                     \
    TOKEN_KEYWORD(Unlock)                                                      \
    TOKEN_KEYWORD(Until)                                                       \
    TOKEN_KEYWORD(Val)                                                         \
    TOKEN_KEYWORD(Variant)                                                     \
    TOKEN_KEYWORD(VarType)                                                     \
    TOKEN_KEYWORD(VbTypeName)                                                  \
    TOKEN_KEYWORD(WeekDay)                                                     \
    TOKEN_KEYWORD(WeekDayName)                                                 \
    TOKEN_KEYWORD(When)                                                        \
    TOKEN_KEYWORD(While)                                                       \
    TOKEN_KEYWORD(With)                                                        \
    TOKEN_KEYWORD(WithEvents)                                                  \
    TOKEN_KEYWORD(Write)                                                       \
    TOKEN_KEYWORD(WriteLine)                                                   \
    TOKEN_KEYWORD(WriteOnly)                                                   \
    TOKEN_KEYWORD(Xor)                                                         \
    TOKEN_KEYWORD(Year)
