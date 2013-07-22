using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LogoCmdSender
{
    class DataMsg
    {
        private const int LOGO_INST_SIZE = 6;
        private const int HEADER_SIZE = 4;

        public enum DataMsgCmd
        {
            LogoCmd = 0, // clears 
            LogoCmd_Append = 1,
            Execute_Mission = 2,
        }

        private DataMsgCmd m_cmdtype;
        private byte m_indexMission;
        private byte m_indexCmd;

        public DataMsg(DataMsgCmd cmdtype, byte indexMission, byte indexCmd)
        {
            m_cmdtype = cmdtype;
            m_indexMission = indexMission;
            m_indexCmd = indexCmd;
        }
        public DataMsg(int cmdtype, byte indexMission, byte indexCmd)
        {
            m_cmdtype = (DataMsgCmd)cmdtype;
            m_indexMission = indexMission;
            m_indexCmd = indexCmd;
        }

        static public byte[] generatepacket(DataMsg msg, MainForm.logoInstructionDef[] logoCmd)
        {
            byte[] packet = new byte[HEADER_SIZE + (LOGO_INST_SIZE * logoCmd.Length)];

            int i = 0;
            packet[i++] = (byte)msg.m_cmdtype;
            packet[i++] = msg.m_indexMission;
            packet[i++] = msg.m_indexCmd;
            packet[i++] = (byte)logoCmd.Length; // instructionCount

            for (int j = 0; j < logoCmd.Length; j++)
            {
                packet[i++] = logoCmd[j].cmd;
                packet[i++] = Convert.ToByte(logoCmd[j].do_fly);
                packet[i++] = Convert.ToByte(logoCmd[j].use_param);
                packet[i++] = logoCmd[j].subcmd;
                packet[i++] = (byte)(logoCmd[j].arg >> 8);
                packet[i++] = (byte)(logoCmd[j].arg & 0xFF);
            }
            return packet;
        }
    }
}
