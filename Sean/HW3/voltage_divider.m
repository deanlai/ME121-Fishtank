function v_out = voltage_divider(v_in, r1, r2)
v_out = v_in.*(r2./(r1+r2));
end