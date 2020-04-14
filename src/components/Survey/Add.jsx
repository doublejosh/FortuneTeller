import React, { useState, useEffect, useRef } from 'react'
import { useSelector } from 'react-redux'
import firebase from '../../firebaseConfigs'
import { css } from 'glamor'

export default () => {
	const theme = useSelector(state => state.theme) || []
	const [fortune, setFortune] = useState()

	const inputRef = useRef(null)
	useEffect(() => {
		inputRef.current.focus()
	})

	return (
		<div
			{...css({
				display: 'grid',
				gridTemplateAreas: `"input input"
									"none button"`,
				gridTemplateColumns: '1fr auto',
				gridRowGap: '2rem',
				padding: '2rem',
			})}>
			<textarea
				id="fortune"
				ref={inputRef}
				onChange={e => setFortune(e.target.value)}
				rows="6"
				{...css({
					gridArea: 'input',
					padding: '2rem',

					[`@media(min-width: ${theme.breaks.md}px)`]: {
						padding: '4rem',
					},

					fontSize: '2rem',
					border: 'none',
					borderRadius: theme.borderRadius,
					outline: 0,
				})}>
				{fortune}
			</textarea>
			<button
				onClick={e => {
					if (fortune) {
						firebase
							.database()
							.ref('fortunes')
							.push({
								text: fortune,
								shown: 0,
								votes: 0,
							})
					}
					setFortune('')
					document.getElementById('fortune').value = ''
				}}
				{...css({
					gridArea: 'button',
					fontSize: '2rem',
					border: 'none',
					padding: '1rem 4rem',
					borderRadius: theme.borderRadius,
					background: '#000',
					color: '#FFF',
					outline: 'none',
					'&:hover': { background: '#333', color: '#DDD' },
					'&:active': { background: '#666', color: '#AAA' },
				})}>
				Add
			</button>
		</div>
	)
}
