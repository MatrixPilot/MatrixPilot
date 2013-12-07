using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LogoCmdSender
{
    class RxLogoParser
    {
        public static String ConvertInstrToString(MainForm.logoInstructionDef instr)
        {
            String result = "UNKNOWN COMMAND";
            byte fly = Convert.ToByte(instr.do_fly);
            byte param = Convert.ToByte(instr.use_param);
            byte sub = instr.subcmd;
            short x = instr.arg;

            switch (instr.cmd)
            {
                case 1:
                    if ((fly == 0) && (sub == 0))
                        result = "_REPEAT(n=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (param == 0) && (sub == 1) && (x == 0))
                        result = "_END";
                    else if ((fly == 0) && (param == 0) && (sub == 3) && (x == 0))
                        result = "_ELSE";
                    else if ((fly == 0) && (param == 0) && (sub == 2))
                        result = "_TO(fn=" + x.ToString() + ")";
                    break;

                case 2:
                    if (fly == 0)
                        result = "_DO(fn=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;

                case 3:
                    if (sub == 0)
                        result = "_FD(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    break;

                case 4:
                    if ((fly == 0) && (sub == 0))
                        result = "_RT(x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 1))
                        result = "_SET_ANGLE(x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 2) && (param == 0) && (x == 0))
                        result = "_USE_CURRENT_ANGLE";
                    else if ((fly == 0) && (sub == 3) && (param == 0) && (x == 0))
                        result = "_USE_ANGLE_TO_GOAL";
                    break;

                case 5:
                    if ((fly == 0) && (sub == 0))
                        result = "_MV_X(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 1))
                        result = "_SET_X(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 2))
                        result = "_MV_Y(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 3))
                        result = "_SET_Y(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 4))
                        result = "_MV_Z(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 5))
                        result = "_SET_Z(x=" + x.ToString() + ", fl=" + fly.ToString() + ", pr=" + param.ToString() + ")";
                    else if ((fly == 0) && (sub == 6) && (param == 0) && (x == 0))
                        result = "_USE_CURRENT_POS(fl=" + fly.ToString() + ")";
                    else if ((fly == 0) && (sub == 7) && (param == 0) && (x == 0))
                        result = "_HOME(fl=" + fly.ToString() + ")";
                    else if ((fly == 0) && (param == 0) && (sub == 8))
                        result = "_SET_ABS_VAL_HIGH(x=" + x.ToString() + ")";
                    else if ((fly == 0) && (param == 0) && (sub == 9))
                        result = "_SET_ABS_X_LOW(x=" + x.ToString() + ")";
                    else if ((param == 0) && (sub == 10))
                        result = "_SET_ABS_Y_LOW(y=" + x.ToString() + ", fl=" + fly.ToString() + ")";
                    break;

                case 6:
                case 7:
                    // TODO
                    break;

                case 8:
                    if ((fly == 0) && (param == 0) && (sub == 0))
                        result = "_SET_TURTLE(x=" + x.ToString() + ")";
                    break;

                case 9:
                    // TODO
                    break;

                case 10:
                    if (fly == 0)
                        result = "_EXEC(fn=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;

                case 11:
                case 12:
                    // TODO
                    break;

                case 13:
                    if ((fly == 0) && (param == 0) && (x == 0))
                        result = "_LOAD_TO_PARAM(val=" + sub.ToString() + ")";
                    break;
                case 14:
                    if (fly == 0)
                        result = "_IF_EO(val=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;
                case 15:
                    if (fly == 0)
                        result = "_IF_NE(val=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;
                case 16:
                    if (fly == 0)
                        result = "_IF_GT(val=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;
                case 17:
                    if (fly == 0)
                        result = "_IF_LT(val=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;
                case 18:
                    if (fly == 0)
                        result = "_IF_GE(val=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;
                case 19:
                    if (fly == 0)
                        result = "_IF_LE(val=" + sub.ToString() + ", x=" + x.ToString() + ", pr=" + param.ToString() + ")";
                    break;

                default:
                    break;
            }

            if (result == "UNKNOWN COMMAND")
            {
                result += "  cmd=" + instr.cmd.ToString();
                result += "  fly=" + fly.ToString();
                result += "  param=" + param.ToString();
                result += "  sub=" + sub.ToString();
                result += "  x=" + x.ToString();
            }

            return result + "\r\n";
        }
    }
}
