import React from 'react'
import { useSelector } from 'react-redux'
import { css } from 'glamor'

export default () => {
	const theme = useSelector(state => state.theme) || []

	return (
		<section
			{...css({
				margin: '0 1.75rem',
				color: '#000',
				textAlign: 'center',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					textAlign: 'left',
				},
				'& h2, & h3': { fontWeight: 'normal' },
			})}>
			<h2 {...css({ fontSize: '1.75rem' })}>
				The machine needs your human wisdom. Help decide the fate of future mortals!
			</h2>
			<h3 {...css({ fontSize: '1.25rem', lineHeight: '1.25rem' })}>
				One quarter and a few yes/no questions grants people with a fortune from the arcade
				machine. They also answer for accuracy, and everthing is saved to the cloud. Adjustment
				and play is planned to calibrate the AI.
			</h3>
		</section>
	)
}
