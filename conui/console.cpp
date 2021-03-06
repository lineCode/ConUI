/*
Team K
- Elliot Kwan - 020 893 129
- Artem Luzyanin - 061 938 114
- Amal Khandlewal - 046 733 127
*/

#include "console.h"

namespace cui
{
bool Console::_insertMode = true;
unsigned int Console::_tabsize = 4;

void Console::strdsp(const char* str, int row, int col, int len, int curpos)
{
    int i = 0;
    setPos(row, col);

    if(len)
    {
        for(; i < len && str[i]; i++)
        {
            putChar(str[i]);
        }

        for(; i < len; i++)
        {
            putChar(' ');
        }
    }
    else
    {
        for(; str[i]; i++)
        {
            putChar(str[i]);
        }
    }

    if(curpos >= 0)
    {
        setPos(row, col + curpos);
    }
}

int Console::stredit(char* str, int row, int col, int fieldLength,
                     int maxStrLength, int* strOffset, int* curPosition,
                     bool InTextEditor, bool ReadOnly, bool& insertMode)
{
    int offsetOriginal = *strOffset;
    int curPosOriginal = *curPosition;
    int localOffset = 0;
    int localCurPos = 0;
    int key = 0;
    int i = 0;
    int strLength = bio::strlen(str);
    bool done = false;
    bool terminate = false;
    // start - record initial values
    char* strOriginal = new char[strLength + 1];
    bio::strcpy(strOriginal, str);
    // end - record initial values

    // start - initial corrections
    if(strOffset == (int*) 0)
    {
        strOffset = &localOffset;
    }
    else if(*strOffset > strLength)
    {
        *strOffset = strLength;
    }

    if(curPosition == (int*) 0)
    {
        curPosition = &localCurPos;
    }
    else if(*curPosition > fieldLength || *curPosition > strLength - *strOffset)
    {
        *curPosition = strLength - *strOffset;

        if(*curPosition >= fieldLength)
        {
            *curPosition = fieldLength - 1;
        }
    }

    // end - initial corrections

    while(!done)
    {
        if(InTextEditor && *strOffset != offsetOriginal)
        {
            done = true;
            terminate = true;
        }
        else
        {
            strdsp(str + *strOffset, row, col, fieldLength, *curPosition);
            key = console.getKey();
            strLength = bio::strlen(str);

            switch(key)
            {
            case LEFT:
                if(*curPosition)
                {
                    (*curPosition)--;
                }
                else if(*strOffset)
                {
                    (*strOffset)--;
                }
                else
                {
                    done = true;
                }

                break;

            case RIGHT:
                if((*curPosition != fieldLength - 1) && (str[*curPosition + *strOffset]))
                {
                    (*curPosition)++;
                }
                else if(*strOffset + fieldLength < strLength + 1)
                {
                    (*strOffset)++;
                }
                else
                {
                    done = true;
                }

                break;

            case HOME:
                *strOffset = *curPosition = 0;
                break;

            case END:
                if(strLength - *strOffset < fieldLength)
                {
                    *curPosition = strLength - *strOffset;
                }
                else
                {
                    *curPosition = fieldLength - 1;
                    *strOffset = strLength - *curPosition;
                }

                break;

            case ESCAPE:
                if(!InTextEditor)
                {
                    bio::strcpy(str, strOriginal);
                    *strOffset = offsetOriginal;
                    *curPosition = curPosOriginal;
                }

                done = true;
                break;

            case BACKSPACE:
                if(!(*curPosition) && !(*strOffset))
                {
                    done = true;
                }
                else if(!ReadOnly)
                {
                    if((*curPosition >= 0 && *strOffset + *curPosition != 0) || (*curPosition == 1 && *strOffset == 0))
                    {
                        for(i = -1; str[*strOffset + *curPosition + i]; i++)
                        {
                            str[*strOffset + *curPosition + i] = str[*strOffset + *curPosition + 1 + i];
                        }

                        (*curPosition)--;
                    }

                    if(*curPosition < 1)
                    {
                        if(fieldLength <= _tabsize && *strOffset > 0)
                        {
                            *curPosition = fieldLength - 1;
                            *strOffset -= fieldLength;
                        }
                        else if(*strOffset >= _tabsize)
                        {
                            *strOffset -= _tabsize;
                            *curPosition += _tabsize;
                        }
                        else
                        {
                            *curPosition += *strOffset;
                            *strOffset = 0;
                        }
                    }
                }

                break;

            case DEL:
                if(!ReadOnly)
                    for(i = 0; str[*strOffset + *curPosition + i]; i++)
                    {
                        str[*strOffset + *curPosition + i] = str[*strOffset + *curPosition + 1 + i];
                    }

                break;

            case INSERT:
                insertMode = !insertMode;
                break;

            case ENTER:
            case UP:
            case DOWN:
            case PGUP:
            case PGDN:
            case F(1):
            case F(2):
            case F(3):
            case F(4):
            case F(5):
            case F(6):
            case F(7):
            case F(8):
            case F(9):
            case F(10):
            case F(11):
            case F(12):
                done = true;
                break;

            case TAB:
                if(!ReadOnly)
                {
                    if(InTextEditor)
                    {
                        if(insertMode)
                        {
                            tabFunction(str, strLength, maxStrLength, *curPosition, *strOffset, _tabsize, fieldLength, true);
                        }
                        else
                        {
                            tabFunction(str, strLength, maxStrLength, *curPosition, *strOffset, _tabsize, fieldLength, false);
                        }
                    }
                    else
                    {
                        done = true;
                    }
                }

                break;

            default:
                if(!ReadOnly)
                {
                    if(key >= ' ' && key <= '~')
                    {
                        if(insertMode)
                        {
                            if(strLength < maxStrLength)
                            {
                                for(i = strLength; i != *curPosition + *strOffset; i--)
                                {
                                    str[i] = str[i - 1];
                                }

                                strLength++;
                                str[*strOffset + *curPosition] = key;

                                if(*curPosition != fieldLength - 1)
                                {
                                    (*curPosition)++;
                                }
                                else if(*curPosition + *strOffset < strLength)
                                {
                                    (*strOffset)++;
                                }
                            }
                        }
                        else if(strLength < maxStrLength || *curPosition + *strOffset < strLength)
                        {
                            strLength < maxStrLength && strLength++;
                            str[*strOffset + *curPosition] = key;

                            if(*curPosition != fieldLength - 1)
                            {
                                (*curPosition)++;
                            }
                            else if(*curPosition + *strOffset < strLength)
                            {
                                (*strOffset)++;
                            }
                        }
                    }

                    str[strLength] = 0;
                    break;
                }   // if
            }       // switch
        }           // else
    }               // while

    /*if(ReadOnly)
    {
        bio::strcpy(str, strOriginal);
    }*/
    delete[] strOriginal;

    if(terminate)
    {
        return 0;
    }

    return key;
}

Console& operator>> (Console& cn, int& ch)
{
    ch = cn.getKey();
    return cn;
}

Console& operator<< (Console& cn, char ch)
{
    cn.putChar(ch);
    return cn;
}

Console& operator<< (Console& cn, const char* str)
{
    for(int i = 0; str[i]; i++)
    {
        cn.putChar(str[i]);
    }

    return cn;
}

void tabFunction(char* str, int& strLength, const int maxStrLength, int& position, int& offset,
                 unsigned int tabsize, const int fieldLength, const bool isInsert)
{
    int i = 0;
    int maxStrCmp = isInsert ? strLength + tabsize : offset + position + tabsize;

    if(maxStrCmp <= maxStrLength)
    {
        if(!isInsert && offset + position + tabsize > strLength)
        {
            str[tabsize + position + offset] = 0;
        }

        if(isInsert)
        {
            for(i = strLength + tabsize; i >= position + offset; i--)
            {
                str[i] = str[i - tabsize];
            }
        }

        for(i = 0; i < tabsize; i++)
        {
            str[i + position + offset] = ' ';
        }

        if(position + tabsize < fieldLength)
        {
            position += tabsize;
        }
        else
        {
            offset += position + tabsize - fieldLength + 1;
            position = fieldLength - 1;
        }

        if(isInsert)
        {
            strLength += tabsize;
        }
    }
    else
    {
        maxStrCmp = isInsert ? strLength : offset + position;

        if(isInsert)
        {
            for(i = maxStrLength; i >= position + offset; i--)
            {
                str[i] = str[i - (maxStrLength - strLength)];
            }
        }

        for(i = 0; i < maxStrLength - maxStrCmp; i++)
        {
            str[i + position + offset] = ' ';
        }

        if(position + maxStrLength - maxStrCmp < fieldLength)
        {
            position += maxStrLength - maxStrCmp;
        }
        else
        {
            offset += position + maxStrLength - strLength - fieldLength + 1;
            position = fieldLength - 1;
        }

        if(!isInsert)
        {
            str[maxStrLength] = 0;
        }
    }
}

Console console;

}