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
            Clear_Mission = 0,
            LogoCmd = 1,
            Execute_Mission = 2,
            Resume_Mission = 3,
            Read_Mission = 4,
            Read_Mission_Response = 5,
            Read_Cmd = 6,
            Read_Cmd_Response = 7,
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

            for (int instIndex = 0; instIndex < logoCmd.Length; instIndex++)
            {
                packet[i++] = logoCmd[instIndex].cmd;
                packet[i++] = Convert.ToByte(logoCmd[instIndex].do_fly);
                packet[i++] = Convert.ToByte(logoCmd[instIndex].use_param);
                packet[i++] = logoCmd[instIndex].subcmd;
                packet[i++] = Convert.ToByte(logoCmd[instIndex].arg >> 8);
                packet[i++] = Convert.ToByte(logoCmd[instIndex].arg & 0xFF);
            }
            return packet;
        }
        
        public static MainForm.logoInstructionDef ConvertPacktToInst(byte[] packet, int offset)
        {
            MainForm.logoInstructionDef inst = new MainForm.logoInstructionDef();

            if (packet.Length < (offset+6)) // offset + length of instr
                return inst;

            ushort arg;

            inst.cmd = packet[offset + 0];
            inst.do_fly = Convert.ToBoolean(packet[offset + 1]);
            inst.use_param = Convert.ToBoolean(packet[offset + 2]);
            inst.subcmd = packet[offset + 3];
            arg = (ushort)packet[offset + 4]; // MSB
            arg <<= 8;
            arg |= (ushort)packet[offset + 5]; // LSB
            inst.arg = (short)arg;
            return inst;
        }
    }
}
