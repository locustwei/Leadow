--[[
[����]:�ٵ�����
[˵��]:����Ʊ������1��̽�׻��������������2��%�����ҹɼ�С�ڣ�����3��Ԫʱ���롣
[Symbol]:��Ʊ����
[nPercent]:�������ȣ��ٷֱ�%��
[nValue]:�����ڣ�Ԫ��
[nVolumn]:���������֣�
--]]


if Symbol==nil then
	return "Symbol = nil"
end

local p = StockData:new(Symbol)

if p.Low == 0 then
	return "p.Low = 0"
end

local n = (p.Current - p.Low) / p.Low        --�ͼۻ����ٷֱ�

if (p.Current < nValue) and (n > nPercent) then
	return p:Buy(nVolumn)                    --���루�֣�
else
	return "����������"
end