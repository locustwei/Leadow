--[[
[����]:��������
[˵��]:����Ʊ������1����߻��䣬��������������2��%�����ҹɼ۴����ڣ�����3��Ԫʱ������
[Symbol]:��Ʊ����
[nPercent]:����������ٷֱ�%��
[nValue]:�ɼ۸��ڣ�Ԫ��
[nVolumn]:���������ɣ�
--]]

if Symbol==nil then
	return "Symbol = nil"
end

local p = StockData:new(Symbol)

if p.Hight == 0 then
	return "p.High = 0"
end

local n = (p.Current - p.High) / p.High        

if (p.Current > nValue) and (n > nPercent) then
	return p:Sell(nVolumn)                    
else
	return "����������"
end