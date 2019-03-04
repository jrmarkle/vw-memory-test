#include <stdio.h>

#include <vowpalwabbit/vwdll.h>

int main() {
	VW_HANDLE h = VW_InitializeA("--loss_function=logistic --confidence --save_resume --link=logistic");

	for (int i = 0; i < 1e3; i++) {
		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Good");
			VW_AddLabel(goodExample, 1, 1, 0);
			VW_Learn(h, goodExample);
			VW_FinishExample(h, goodExample);
		}

		{
			VW_EXAMPLE badExample = VW_ReadExampleA(h, "|Feature Bad");
			VW_AddLabel(badExample, -1, 1, 0);
			VW_Learn(h, badExample);
			VW_FinishExample(h, badExample);
		}
	}

	{
		VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Good");
		float prediction = VW_Learn(h, goodExample);
		float confidence = VW_GetConfidence(goodExample);
		printf("predict good = %f, confidence = %f\n", prediction, confidence);
		VW_FinishExample(h, goodExample);
	}
	{
		VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Bad");
		float prediction = VW_Learn(h, goodExample);
		float confidence = VW_GetConfidence(goodExample);
		printf("predict bad = %f, confidence = %f\n", prediction, confidence);
		VW_FinishExample(h, goodExample);
	}
	{
		VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Unknown");
		float prediction = VW_Learn(h, goodExample);
		float confidence = VW_GetConfidence(goodExample);
		printf("predict unknown = %f, confidence = %f\n", prediction, confidence);
		VW_FinishExample(h, goodExample);
	}

	VW_Finish(h);
	return 0;
}
